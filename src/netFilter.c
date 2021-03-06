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
#include <linux/netfilter_bridge.h>

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
   // nfho_single.hooknum = NF_INET_PRE_ROUTING;  // 数据流入前触发
    nfho_single.hooknum = NF_BR_PRE_ROUTING;    // 数据流入网桥前触发
    //nfho_single.pf = PF_INET;
    nfho_single.pf = PF_BRIDGE;
    //nfho_single.priority = NF_IP_PRI_FILTER;
    nfho_single.priority = NF_BR_PRI_FIRST;

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
char mymessagebuf[100000];  // 放置缓冲区定义
char tcp_udp_body[100000];  // 记录应用层数据
char xmldest[100000]; //dhy
struct completion cmpl; //同步机制 dai
extern int operatorflag;

#define DEBUG(...) sprintf(mymessagebuf, "DEBUG:"__VA_ARGS__);sendMsgNetlink(mymessagebuf);
#define INFO(...) sprintf(mymessagebuf, "INFO:"__VA_ARGS__);sendMsgNetlink(mymessagebuf);
#define WARNING(...) sprintf(mymessagebuf, "WARNING:"__VA_ARGS__);sendMsgNetlink(mymessagebuf);
#define ERROR(...) sprintf(mymessagebuf, "ERROR:"__VA_ARGS__);sendMsgNetlink(mymessagebuf);

unsigned int hook_func(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in,
                       const struct net_device *out, int (*okfn)(struct sk_buff *)) {
    // 单向拦截数据的钩子函数

    char *data; // data是数据指针游标，从skb->data表示的ip数据报开始

    struct ethhdr *eth; // 以太网帧首部指针

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

    eth = eth_hdr(skb); // 获得以太网帧首部指针
    iph = ip_hdr(skb);  // 获得ip数据报首部指针，或者iph = (struct iphdr *) data;

    if(!skb || !iph || !eth || !skb->data)
        return NF_ACCEPT;

    // 过滤掉广播数据
    if(skb->pkt_type==PACKET_BROADCAST)
        return NF_ACCEPT;

    data = skb->data;   // 将data指向ip数据报首部

//    DEBUG("skb->len=%d, skb->data_len=%d", skb->len, skb->data_len);
//    DEBUG("skb->mac_len=%d", skb->mac_len);
//    DEBUG("skb->head=%x,skb->data=%x,skb->tail=%u,skb->end=%u", skb->head, skb->data, skb->tail, skb->end);
//    DEBUG("skb_mac_header=%x,skb_network_header=%x,skb_transport_header=%x", skb_mac_header(skb), skb_network_header(skb), skb_transport_header(skb));

    ip_head_len = iph->ihl * 4;     // 获得首部长度
    ip_body_len = ntohs(iph->tot_len) - ip_head_len;   //获得数据部分长度,注意总长度为网络大端序，需转成小端序

    if (iph->saddr != in_aton(sourceip)
        || iph->daddr != in_aton(targetip)) {
        // 比较配置中ip与获取ip的16进制形式
        return NF_ACCEPT;
    }

    data += ip_head_len;    // 将data指向TCP/UDP报文首部

    switch (iph->protocol) {    // 根据TCP还是UDP进行不同的处理
        case IPPROTO_ESP:
        case IPPROTO_AH:
            DEBUG("ESP AND AH:%s ---> %s", in_ntoa(sip, iph->saddr), in_ntoa(dip, iph->daddr));
            break;
        case IPPROTO_TCP: {
            //获取tcp头，并计算其长度
            tcphead = (struct tcphdr *) data;
            tcp_head_len = tcphead->doff * 4;
            tcp_body_len = ip_body_len - tcp_head_len;

            data += tcp_head_len;   // 将data指向TCP数据部分
            strncpy(tcp_udp_body, data, tcp_body_len);
            tcp_udp_body[tcp_body_len] = '\0';

            DEBUG("TCP:%s:%d ---> %s:%d::%s", in_ntoa(sip, iph->saddr), ntohs(tcphead->source),
                  in_ntoa(dip, iph->daddr), ntohs(tcphead->dest), tcp_udp_body);

            //tcp body长度小于最小要求长度，直接通过
            if (tcp_body_len < MIN_SIZE)
                return NF_ACCEPT;
            break;
        }
        case IPPROTO_UDP: {
            //获取udp头部，并计算其长度
            udphead = (struct udphdr *) data;
            udp_head_len = sizeof(struct udphdr);
            udp_body_len = ntohs(udphead->len) - udp_head_len;

            data += udp_head_len;   // 将data指向UDP数据部分
            strncpy(tcp_udp_body, data, udp_body_len);
            tcp_udp_body[udp_body_len] = '\0';


            // udp body长度小于最小要求长度，直接通过
            //if (udp_body_len < MIN_SIZE)
            //    return NF_ACCEPT;

	    //对于message标记中内容为B的信息包，不予通过。
            extract(xmldest, "message", tcp_udp_body, 0, 100);
            if(strcmp(xmldest,"B")==0)
		{
                 //sendMsgNetlink("B"); 
               DEBUG("UDP:%s:%d ---> %s:%d::%s", in_ntoa(sip, iph->saddr), ntohs(udphead->source),
               in_ntoa(dip, iph->daddr), ntohs(udphead->dest),tcp_udp_body);
		if(operatorflag == 2)
		{
			printk("netfilter:舍弃\n");
			return NF_DROP;
		}
		//init_completion(&cmpl);
		//wait_for_completion_timeout(&cmpl,3 * HZ);
                
		}
		//check flag
                  //
                 //if flag 
               //   accept  drop 

            //DEBUG("UDP:%s:%d ---> %s:%d::%s", in_ntoa(sip, iph->saddr), ntohs(udphead->source),
                  //in_ntoa(dip, iph->daddr), ntohs(udphead->dest),tcp_udp_body);

            break;
        }
        case IPPROTO_ICMP:{
            // icmp协议
            DEBUG("ICMP:%s ---> %s", in_ntoa(sip, iph->saddr), in_ntoa(dip, iph->daddr));
            break;
        }
        case IPPROTO_IGMP:{
            DEBUG("IGMP:%s ---> %s", in_ntoa(sip, iph->saddr), in_ntoa(dip, iph->daddr));
            break;
        }
        case IPPROTO_PUP:{
            DEBUG("PUP:%s ---> %s", in_ntoa(sip, iph->saddr), in_ntoa(dip, iph->daddr));
            break;
        }
        case IPPROTO_IDP:{
            DEBUG("IDP:%s ---> %s", in_ntoa(sip, iph->saddr), in_ntoa(dip, iph->daddr));
            break;
        }
        case IPPROTO_IP:{
            DEBUG("IP:%s ---> %s", in_ntoa(sip, iph->saddr), in_ntoa(dip, iph->daddr));
            break;
        }
        case IPPROTO_RAW:{
            DEBUG("RAW:%s ---> %s", in_ntoa(sip, iph->saddr), in_ntoa(dip, iph->daddr));
            break;
        }
        case IPPROTO_GRE: {
            DEBUG("GRE:%s ---> %s", in_ntoa(sip, iph->saddr), in_ntoa(dip, iph->daddr));
            break;
        }
        default: {  // 如果有其他可能情况，给出提示
            DEBUG("Other Protocol=%d, %s ---> %s", iph->protocol, in_ntoa(sip, iph->saddr), in_ntoa(dip, iph->daddr));
            break;
        }
    }

    return NF_ACCEPT;
}


