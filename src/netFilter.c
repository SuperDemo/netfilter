#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
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
#include <linux/string.h>

#include "log.h"
#include "netFilter.h"
#include "dealConf.h"
#include "netLink.h"

#define MIN_SIZE 96 //符合给定schema的最小数据包长度

static struct nf_hook_ops nfho_single;  // netfilter钩子

static unsigned int count = 0; //记录过滤的数据包数目

int initNetFilter(void){
    // 初始化netfilter

    nfho_single.hook = (nf_hookfn *) hook_func;   // 绑定钩子函数
    nfho_single.hooknum = NF_INET_PRE_ROUTING;  // 数据流入前触发
    nfho_single.pf = PF_INET;
    nfho_single.priority = NF_IP_PRI_FILTER;

    if (strcmp(direction, "=>") == 0) {  // 如果选择单向拦截
        DEBUG("one way intercept\n");
        nf_register_hook(&nfho_single); //注册一个netfilter钩子
    } else if (strcmp(direction, "<=>") == 0) {    //如果选择双向拦截
        DEBUG("two way intercept\n");
    }

    return 0;
}

int releaseNetFilter(void){
    // 释放netfilter钩子

    if (strcmp(direction, "=>") == 0) {   // 如果选择单向拦截
        DEBUG("one way intercept\n");
        nf_unregister_hook(&nfho_single);   // 卸载钩子
    } else if (strcmp(direction, "<=>") == 0) {     // 如果选择双向拦截
        DEBUG("two way intercept\n");
    }

    return 0;
}

// 在这里取消定义前面展开的宏，并给予新的定义
#undef DEBUG
#undef INFO
#undef WARNING
#undef ERROR

#include <linux/string.h>
char mymessagebuf[1100];  // 放置缓冲区定义
char tcp_udp_body[1000];  // 记录应用层数据

#define DEBUG(...) sprintf(mymessagebuf, "DEBUG:"__VA_ARGS__);sendMsgNetlink(mymessagebuf);
#define INFO(...) sprintf(mymessagebuf, "INFO:"__VA_ARGS__);sendMsgNetlink(mymessagebuf);
#define WARNING(...) sprintf(mymessagebuf, "WARNING:"__VA_ARGS__);sendMsgNetlink(mymessagebuf);
#define ERROR(...) sprintf(mymessagebuf, "ERROR:"__VA_ARGS__);sendMsgNetlink(mymessagebuf);

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

//    DEBUG("skb->len=%d, skb->data_len=%d", skb->len, skb->data_len);
//    DEBUG("skb->mac_len=%d", skb->mac_len);
//    DEBUG("skb->head=%x,skb->data=%x,skb->tail=%u,skb->end=%u", skb->head, skb->data, skb->tail, skb->end);
//    DEBUG("skb_mac_header=%x,skb_network_header=%x,skb_transport_header=%x", skb_mac_header(skb), skb_network_header(skb), skb_transport_header(skb));

    iph = (struct iphdr *) data;    // 获得ip数据报首部指针
    ip_head_len = iph->ihl * 4;     // 获得首部长度
    ip_body_len = ntohs(iph->tot_len) - ip_head_len;   //获得数据部分长度,注意总长度为网络大端序，需转成小端序

//    DEBUG("ip_head_len=%d,ip_body_len=%d", ip_head_len, ip_body_len);

//    if (iph->saddr == in_aton(sourceip)
//        && iph->daddr == in_aton(targetip)) {
//        // 比较配置中ip与获取ip的16进制形式
//        return NF_ACCEPT;
//    }

//    DEBUG("%s ---> %s", in_ntoa(sip, iph->saddr), in_ntoa(dip, iph->daddr));

    data += ip_head_len;    // 将data指向TCP/UDP报文首部

    switch (iph->protocol) {    // 根据TCP还是UDP进行不同的处理
        case IPPROTO_TCP: {
            //获取tcp头，并计算其长度
            tcphead = (struct tcphdr *) data;
            tcp_head_len = tcphead->doff * 4;
            tcp_body_len = ip_body_len - tcp_head_len;
//            INFO("tcp_head_len=%d, tcp_body_len=%d, source port=%d, dest port=%d", tcp_head_len, tcp_body_len,
//            ntohs(tcphead->source), ntohs(tcphead->dest));

            //tcp body长度小于最小要求长度，直接通过
            if (tcp_body_len < MIN_SIZE)
                return NF_ACCEPT;

            data += tcp_head_len;   // 将data指向TCP数据部分

            strncpy(tcp_udp_body, data, tcp_body_len);
            tcp_udp_body[tcp_body_len] = '\0';

            DEBUG("tcpdata:%s\n", tcp_udp_body);

            break;
        }
        case IPPROTO_UDP: {
            //获取udp头部，并计算其长度
            udphead = (struct udphdr *) data;
            udp_head_len = sizeof(struct udphdr);
            udp_body_len = ntohs(udphead->len) - udp_head_len;
//            INFO("udp_head_len=%d, udp_body_len=%d", udp_head_len, udp_body_len);

            // udp body长度小于最小要求长度，直接通过
            if (udp_body_len < MIN_SIZE)
                return NF_ACCEPT;

            data += udp_head_len;   // 将data指向UDP数据部分

//            strncpy(tcp_udp_body, data, 10);
//            tcp_udp_body[10] = '\0';
//            DEBUG("udpdata:%s", tcp_udp_body);

            break;
        }
        case IPPROTO_ICMP:{
            // icmp协议
//            INFO(" icmp protocol");
        }
        default: {  // 如果有其他可能情况，给出提示
//            DEBUG("Other TranLayer proto=%d, with IPPROTO_TCP=%d, with IPPROTO_UDP=%d", iph->protocol, IPPROTO_TCP,
//                  IPPROTO_UDP);
            return NF_ACCEPT;
        }
    }

    //DEBUG("data:%c%c%c%c", data[0], data[1], data[2], data[3]);




    return NF_ACCEPT;

    // 检查数据部分合法性
    if (isLegal(data)) {
        DEBUG("data is legal");
        extract(title, "title", data, 0, CONTENTMAXLEN);   // 提取出title

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
                    extract(message, "message", data, 0, CONTENTMAXLEN);
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
    } else {
        //DEBUG("data is illegal");
    }

    return NF_ACCEPT;
}
