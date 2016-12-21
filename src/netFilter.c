#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/netfilter.h>
#include<linux/netfilter_ipv4.h>
#include<linux/ip.h>
#include<linux/inet.h>
#include<linux/skbuff.h>
#include<linux/tcp.h>
#include<linux/udp.h>
#include<linux/time.h>
#include <linux/timer.h>
#include <linux/timex.h>
#include <linux/rtc.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include<linux/string.h>

#include "log.h"
#include "netFilter.h"
#include "dealConf.h"
#include "netLink.h"

static struct nf_hook_ops nfho_single;  // netfilter钩子

#define MIN_SIZE 96 //符合给定schema的最小数据包长度

// 钩子函数声明
unsigned int hook_func(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in,
                       const struct net_device *out, int (*okfn)(struct sk_buff *));

unsigned static int count = 0; //记录过滤的数据包数目

char direction[50];
char titlecontent[50];
char content_flag[20];
char isapi[50];
char content[50];
char action[50];
char sourceip[50];
char targetip[50];

char mymessagebuf[100] = "from netFilter!";

unsigned int hook_func(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in,
                       const struct net_device *out, int (*okfn)(struct sk_buff *)) {
    // 单向拦截数据的钩子函数

    char *data; // data是数据指针游标，从skb->data表示的ip数据报开始

    struct iphdr *iph;  // ip数据报首部指针
    int ip_head_len;    // 首部长度
    int ip_body_len;    // 数据长度
    char sip[100], dip[100];    // 记录字符串格式的源ip和目的ip的临时空间

    struct tcphdr *tcphead; // tcp报文首部指针
    int tcp_head_len;   // 首部长度
    int tcp_body_len;   // 数据长度

    struct udphdr *udphead; // udp报文首部长度
    int udp_head_len;   // 首部长度
    int udp_body_len;   // 数据长度

    char message[50]; // 记录message
    char title[50]; // 记录抽象事件——主题

    if (!skb || !skb->data) return NF_ACCEPT;
    data = skb->data;   // 将data指向ip数据报首部

    iph = (struct iphdr *) data;    // 获得ip数据报首部指针
    ip_head_len = iph->ihl * 4;     // 获得首部长度
    ip_body_len = iph->tot_len - ip_head_len;   //获得数据部分长度

    sprintf(mymessagebuf, "sprintf test");
    sendMsgNetlink(mymessagebuf);

    if (iph->saddr != in_aton(sourceip)
        || iph->daddr != in_aton(targetip)) {
        // 比较配置中ip与获取ip的16进制形式
        return NF_ACCEPT;
    }

    // 显示捕获的ip数据报的点分10进制形式
    DEBUG("%s ---> %s\n", in_ntoa(sip, iph->saddr), in_ntoa(dip, iph->daddr));
//    strcpy(mymessagebuf, in_ntoa(sip, iph->saddr));
//    strcat(mymessagebuf, " ---> ");
//    strcat(mymessagebuf, in_ntoa(dip, iph->daddr));
//    sendMsgNetlink(mymessagebuf);

    data += ip_head_len;    // 将data指向TCP/UDP报文首部

    switch (iph->protocol) {    // 根据TCP还是UDP进行不同的处理
        case IPPROTO_TCP: {
            //获取tcp头，并计算其长度
            tcphead = (struct tcphdr *) data;
            tcp_head_len = tcphead->doff * 4;
            tcp_body_len = ip_body_len - tcp_head_len;
            INFO("tcp_head_len=%d, tcp_body_len=%d\n", tcp_head_len, tcp_body_len);

            //tcp body长度小于最小要求长度，直接通过
            if (tcp_body_len < MIN_SIZE)
                return NF_ACCEPT;

            data += tcp_head_len;   // 将data指向TCP数据部分
            break;
        }
        case IPPROTO_UDP: {
            //获取udp头部，并计算其长度
            udphead = (struct udphdr *) data;
            udp_head_len = sizeof(struct udphdr);
            udp_body_len = udphead->len - udp_head_len;
            INFO("udp_head_len=%d, udp_body_len=%d\n", udp_head_len, udp_body_len);

            data += udp_head_len;   // 将data指向UDP数据部分
            break;
        }
        default: {  // 如果有其他可能情况，给出提示
            DEBUG("Other TranLayer proto=%d, with IPPROTO_TCP=%d, with IPPROTO_UDP=%d\n", iph->protocol, IPPROTO_TCP,
                  IPPROTO_UDP);
            return NF_ACCEPT;
        }
    }

    // 检查数据部分合法性
    if (isLegal(data)) {
        extract(title, "title", data, 0);   // 提取出title

        INFO("title is:%s\n", title);

        count++;

        if (strcmp(isapi, "no") == 0) {
            if (strcmp(content, "") != 0) {
                if (strcmp(content_flag, "all") == 0) {
                    if (strstr(data, content)) {
                        if (strcmp(action, "drop") == 0) {
                            return NF_DROP;
                        } else if (strcmp(action, "alert") == 0) {
                            return NF_ACCEPT;
                        } else {
                            return NF_ACCEPT;
                        }
                    }
                }
                    //暂时先匹配 message待加
                else if (strcmp(content_flag, "message") == 0) {
                    extract(message, "message", data, 0);
                    if (strstr(message, content)) {
                        if (strcmp(action, "drop") == 0) {
                            // sendnlmsg(title);
                            return NF_DROP;
                        } else if (strcmp(action, "alert") == 0) {
                            // sendnlmsg(title);
                            return NF_ACCEPT;
                        } else {
                            return NF_ACCEPT;
                        }
                    }
                }
            }
            if (strcmp(titlecontent, "") != 0) {
                if (strstr(title, titlecontent)) {
                    if (strcmp(action, "drop") == 0) {
                        // sendnlmsg(title);
                        return NF_DROP;
                    } else if (strcmp(action, "alert") == 0) {
                        // sendnlmsg(title);
                        return NF_ACCEPT;
                    } else {
                        return NF_ACCEPT;
                    }
                }
            }
        }
    }

    return NF_ACCEPT;
}

static int __init init(void){
    // 插入模块时

    char *readFileData;

    INFO("加载netfilter模块！\n");

    // 从配置文件中读取配置
    readFileData = readConf();
    if (!readFileData){
        WARNING("cannot readConf!\n");
        return 1;
    }
    else{
        DEBUG("readConf: %s\n", readFileData);
    }

    // 解析配置
    extract(direction, "direction", readFileData, 0);
    extract(content_flag, "content_flag", readFileData, 0);
    extract(content, "content", readFileData, 0);
    extract(sourceip, "sourceip", readFileData, 0);
    extract(targetip, "targetip", readFileData, 0);
    extract(action, "action", readFileData, 0);
    extract(titlecontent, "titlecontent", readFileData, 0);
    extract(isapi, "isapi", readFileData, 0);

    DEBUG("titlecontent:%s, "
                  "direction:%s, "
                  "sourceip:%s, "
                  "targetip:%s, "
                  "action:%s, "
                  "content_flag:%s, "
                  "content:%s, "
                  "isapi:%s\n",
          titlecontent, direction, sourceip, targetip, action, content_flag, content, isapi);

    createNetlink();    // 初始化netlink模块

    nfho_single.hook = (nf_hookfn *) hook_func;   // 绑定钩子函数
    nfho_single.hooknum = NF_INET_PRE_ROUTING;  // 数据流入前触发
    nfho_single.pf = PF_INET;
    nfho_single.priority = NF_IP_PRI_FILTER;

    if (strcmp(direction, "=>") == 0) {  // 如果选择单向拦截
        DEBUG("单向拦截\n");
        nf_register_hook(&nfho_single); //注册一个netfilter钩子
    } else if (strcmp(direction, "<=>") == 0) {    //如果选择双向拦截
        DEBUG("双向拦截\n");
    }

    return 0;
}

static void __exit fini(void){
    // 移除模块时

    INFO("移除netfilter模块！\n");

    deleteNetlink();    // 释放netlink

    if (strcmp(direction, "=>") == 0) {   // 如果选择单向拦截
        DEBUG("单向拦截\n");
        nf_unregister_hook(&nfho_single);   // 卸载钩子
    } else if (strcmp(direction, "<=>") == 0) {     // 如果选择双向拦截
        DEBUG("双向拦截\n");
    }

}

module_init(init);  // 模块入口，插入模块后调用绑定函数
module_exit(fini);  // 模块出口，插入模块后调用绑定函数
