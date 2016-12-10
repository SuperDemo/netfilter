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

static struct nf_hook_ops nfho_single;

#define MIN_SIZE 96 //符合给定schema的最小数据包长度

////钩子函数声明
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

unsigned int hook_func(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in,
                       const struct net_device *out, int (*okfn)(struct sk_buff *)) {

    char *data; //ip layer data

    struct iphdr *iph;  //ip layer
    int ip_head_len;
    int ip_body_len;
    char sip[100], dip[100];

    struct tcphdr *tcphead; //tcp layer
    int tcp_head_len;
    int tcp_body_len;

    struct udphdr *udphead; //udp layer
    int udp_head_len;
    int udp_body_len;

    char message[50]; //记录message
    char title[50]; //记录抽象事件==主题

    if (!skb) return NF_ACCEPT;
    data = skb->data;
    if (!data) return NF_ACCEPT;

    //iph = ip_hdr(skb);
    iph = (struct iphdr *) data;
    ip_head_len = iph->ihl * 4;
    ip_body_len = iph->tot_len - ip_head_len;

    if (iph->saddr != in_aton(sourceip)
        || iph->daddr != in_aton(targetip)) {
        //DEBUG("%s ---> %s\n", in_ntoa(sip, iph->saddr), in_ntoa(dip, iph->daddr));
        return NF_ACCEPT;
    }

    data += ip_head_len;

    DEBUG("%s ---> %s\n", in_ntoa(sip, iph->saddr), in_ntoa(dip, iph->daddr));

    switch (iph->protocol) {
        case IPPROTO_TCP: {
            //获取tcp头，并计算其长度
            tcphead = (struct tcphdr *) data;
            tcp_head_len = tcphead->doff * 4;
            tcp_body_len = ip_body_len - tcp_head_len;
            INFO("tcp_head_len=%d,tcp_body_len=%d\n", tcp_head_len, tcp_body_len);

            //tcp body长度小于最小要求长度，直接通过
            if (tcp_body_len < MIN_SIZE)
                return NF_ACCEPT;

            data += tcp_head_len;
            break;
        }
        case IPPROTO_UDP: {
            //获取udp头部，并计算其长度
            udphead = (struct udphdr *) data;
            udp_head_len = sizeof(struct udphdr);
            udp_body_len = udphead->len - udp_head_len;
            INFO("udp_head_len=%d,udp_body_len=%d\n", udp_head_len, udp_body_len);

            data += udp_head_len;
            break;
        }
        default: {
            DEBUG("proto=%d,IPPROTO_TCP=%d,IPPROTO_UDP=%d\n", iph->protocol, IPPROTO_TCP, IPPROTO_UDP);
            return NF_ACCEPT;
        }
    }

    if (isLegal(data)) {
        extract(title, "title", data, 0);

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

    char *readFileData;

    INFO("加载netfilter模块！\n");

    readFileData = readConf();
    if (!readFileData){
        WARNING("cannot readConf!\n");
        return 1;
    }
    else{
        DEBUG("readConf: %s\n", readFileData);
    }

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

    nfho_single.hook = (nf_hookfn*)hook_func;   //bound hook function
    nfho_single.hooknum = NF_INET_PRE_ROUTING;  //pre routing
    nfho_single.pf = PF_INET;
    nfho_single.priority = NF_IP_PRI_FILTER;

    if (strcmp(direction, "=>") == 0){  //如果选择单向拦截
        DEBUG("单向拦截\n");
        nf_register_hook(&nfho_single);
    }
    else if (strcmp(direction, "<=>") == 0){
        DEBUG("双向拦截\n");
    }

    return 0;
}

static void __exit fini(void){

    INFO("移除netfilter模块！\n");

    if (strcmp(direction, "=>") == 0) {   //如果选择单向拦截
        DEBUG("单向拦截\n");
        nf_unregister_hook(&nfho_single);
    } else if (strcmp(direction, "<=>") == 0) {
        DEBUG("双向拦截\n");
    }

}

module_init(init);
module_exit(fini);
