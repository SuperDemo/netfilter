#include <linux/init.h>  
#include <linux/module.h>  
#include <linux/timer.h>  
#include <linux/time.h>  
#include <linux/types.h>  
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/kernel.h>
#include <linux/string.h>

#include "log.h"
#include "netLink.h"

static struct sock *nl_sk;  // 内核套接字
static struct netlink_kernel_cfg cfg;   // netlink内核配置参数
static int pid;    // 客户端pid

static void recvMsgNetlink(struct sk_buff *skb) {
    // 当netlink上接收到消息时触发此函数

    struct nlmsghdr *nlh;   // 指向netlink消息首部的指针
    char str[100];  // 存放消息的缓冲区

    DEBUG("recvMsgNetlink is triggerd\n");

    if (skb->len >= NLMSG_SPACE(0)) {

        nlh = nlmsg_hdr(skb);   // 获取netlink消息首部指针

        memcpy(str, NLMSG_DATA(nlh), sizeof(str));  // 获取netlink消息主体
        INFO("netlink message received:%s\n", str);

        pid = nlh->nlmsg_pid;   // 获取消息中客户端进程pid
        INFO("netlink client pid is: %d \n", pid);

        sendMsgNetlink("msg from kernel!");  // 向客户端发送回复消息
    }
}


void sendMsgNetlink(char *message) {
    // 通过netlink向进程号为pid的用户态进程发送消息message

    struct sk_buff *skb;    // 定义套接字缓冲区
    struct nlmsghdr *nlh;
    int len = strlen(message) + 1;

    if (!message || !nl_sk) return;

    skb = alloc_skb(len, GFP_KERNEL); //申请一个skb,长度为len,优先级为GFP_KERNEL
    if (!skb) {
        ERROR("my_net_link:alloc_skb_1 error\n");
    }

    nlh = nlmsg_put(skb, 0, 0, 0, len, 0);  // 设置netlink消息头部为nlh

    //NETLINK_CB(skb).pid = 0;  // 消息发送者为内核，所以pid为0
    //NETLINK_CB(skb).dst_pid = pid;  // 消息接收者的pid
    NETLINK_CB(skb).dst_group = 0;    // 目标为进程时，设置为0

    memcpy(NLMSG_DATA(nlh), message, strlen(message) + 1);  // 填充nlh为具体消息
    DEBUG("my_net_link:send message '%s'.\n", (char *) NLMSG_DATA(nlh));

    // 发送单播消息，参数分别为nl_sk(内核套接字), skb(套接字缓冲区), pid(目的进程), MSG_DONTWAIT(不阻塞)
    netlink_unicast(nl_sk, skb, pid, MSG_DONTWAIT); // 发送单播消息
}

int createNetlink(void) {
    // 在内核中创建netlink，当用户态传来消息时触发绑定的接收消息函数

    // kernel 2.6
    // nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST, 1, nl_data_ready, NULL, THIS_MODULE);

    // kernel 3.10
    cfg.groups = 0; // 0表示单播，1表示多播
    cfg.flags = 0;
    cfg.input = recvMsgNetlink;    // 回调函数，当收到消息时触发
    cfg.cb_mutex = NULL;

    // 创建服务，init_net表示网络设备命名空间指针，NETLINK_TEST表示协议类型，cfg指向netlink的配置结构体
    nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST, &cfg);
    if (!nl_sk) {
        ERROR("my_net_link: create netlink socket error.\n");
        return 1;
    }
    INFO("my_net_link: create netlink socket ok.\n");

    return 0;
}

int deleteNetlink(void) {
    // 释放netlink

    if (nl_sk) {
        netlink_kernel_release(nl_sk);
    }

    INFO("my_net_link: netlink socket released\n");

    return 0;
}