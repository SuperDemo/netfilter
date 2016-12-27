#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>
#include <errno.h>
#include <signal.h>

#include "netlink_client.h"

struct netlink_message {
    struct nlmsghdr hdr;    // netlink的消息首部
    struct packet_info mymessage;    // 额外消息内容
} recv_buf;

static int sock_fd;     // 客户端套接字
static struct sockaddr_nl dest_addr;    // 内核目标地址数据结构

static void quit(int signum){
    // 退出netlink客户端不仅要关闭套接字，还需要先往内核发送消息表示应用程序即将关闭

    // 设置待发送的netlink消息
    struct nlmsghdr message;
    memset(&message, 0, sizeof(message));
    message.nlmsg_len = NLMSG_LENGTH(0);    // netlink消息的总长度,仅需提供发送数据的长度，由该宏自动计算对齐后的总长度
    message.nlmsg_flags = 0;   // 用户应用内部定义消息的类型，大部分情况下设置为0
    message.nlmsg_type = NETLINK_TEST_DISCONNECT;   // 设置消息标志，此处设置为断开连接时标志
    message.nlmsg_pid = getpid();

    // 向内核发消息，由nlmsg_type表明，应用程序即将关闭
    if (sendto(sock_fd, &message, message.nlmsg_len, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0){
        printf("send disconnect message to kernel failed!\n");
    }

    close(sock_fd); // 关闭套接字
    exit(0);
}

int main(int argc, char *argv[]) {

    // 创建客户端原始套接字，协议为NETLINK_TEST
    if ((sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_TEST)) < 0) {
        printf("can't create netlink socket!\n");
        return -1;
    }

    // 创建本地地址数据结构
    struct sockaddr_nl local;
    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK;
    local.nl_pid = getpid();
    local.nl_groups = 0;

    // 将套接字与本地地址绑定
    if (bind(sock_fd, (struct sockaddr*)&local, sizeof(local)) != 0){
        printf("bind() error!\n");
        return -1;
    }

    // 设置目标地址数据结构
    memset(&dest_addr, 0, sizeof(dest_addr));   //清空
    dest_addr.nl_family = AF_NETLINK;   // 设置协议簇
    dest_addr.nl_pid = 0;   // 接收或发送消息的进程的id，内核处理消息所以设置为0
    dest_addr.nl_groups = 0;    // 指定多播组，设置为0表示调用者不加入任何多播组

    // 设置待发送的netlink消息
    struct nlmsghdr message;
    memset(&message, 0, sizeof(message));   // 清空该消息
    message.nlmsg_len = NLMSG_LENGTH(0);    // netlink消息的总长度,仅需提供发送数据的长度，由该宏自动计算对齐后的总长度
    message.nlmsg_flags = 0;   // 用户应用内部定义消息的类型，大部分情况下设置为0
    message.nlmsg_type = NETLINK_TEST_CONNECT;   // 设置消息标志，这里设置为连接时标志
    message.nlmsg_pid = getpid();   // 设置发送者的PID

    // 发送netlink消息到内核
    if (sendto(sock_fd, &message, message.nlmsg_len, 0, (struct sockaddr *) &dest_addr, sizeof(dest_addr)) < 0) {
        printf("send connect message to kernel failed!\n");
        return -1;
    }

    // 先设置信号函数，用于退出死循环
    signal(SIGINT, quit);

    // 接收来自内核的netlink消息
    while (1) {
        int dest_addr_len = sizeof(struct sockaddr_nl);
        if (recvfrom(sock_fd, &recv_buf, sizeof(recv_buf), 0, (struct sockaddr *) &dest_addr, &dest_addr_len) < 0) {
            printf("recv message from kernel failed!\n");
        } else {
            printf("Get messages:%d\n", recv_buf.mymessage.size);
        }
    }

    return 0;
}

