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

#include "log_message.h"
#include "netFilter.h"
#include "dealConf.h"
#include "netLink.h"


extern char direction[50];
extern char titlecontent[50];
extern char content_flag[20];
extern char isapi[50];
extern char content[50];
extern char action[50];
extern char sourceip[50];
extern char targetip[50];

static char TUMessage[1000];

extern struct nf_hook_ops nfho_single;  // netfilter钩子

unsigned static int count = 0; //记录过滤的数据包数目

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

    //DEBUG("sprintf test: %s", in_ntoa(sip, iph->saddr));
    // 显示捕获的ip数据报的点分10进制形式
    //DEBUG("\n%s ---> %s", in_ntoa(sip, iph->saddr), in_ntoa(dip, iph->daddr));

    if (iph->saddr == in_aton(sourceip)
        && iph->daddr == in_aton(targetip)) {
        // 比较配置中ip与获取ip的16进制形式
        return NF_ACCEPT;
    }

    // 显示捕获的ip数据报的点分10进制形式
    DEBUG("\n%s ---> %s", in_ntoa(sip, iph->saddr), in_ntoa(dip, iph->daddr));

    data += ip_head_len;    // 将data指向TCP/UDP报文首部

    switch (iph->protocol) {    // 根据TCP还是UDP进行不同的处理
        case IPPROTO_TCP: {
            //获取tcp头，并计算其长度
            tcphead = (struct tcphdr *) data;
            tcp_head_len = tcphead->doff * 4;
            tcp_body_len = ip_body_len - tcp_head_len;
            //INFO("tcp_head_len=%d, tcp_body_len=%d\n", tcp_head_len, tcp_body_len);

            //tcp body长度小于最小要求长度，直接通过
            if (tcp_body_len < MIN_SIZE)
                return NF_ACCEPT;

            data += tcp_head_len;   // 将data指向TCP数据部分

            //strncpy(TUMessage, data, tcp_body_len);
            break;
        }
        case IPPROTO_UDP: {
            //获取udp头部，并计算其长度
            udphead = (struct udphdr *) data;
            udp_head_len = sizeof(struct udphdr);
            udp_body_len = udphead->len - udp_head_len;
            //INFO("udp_head_len=%d, udp_body_len=%d\n", udp_head_len, udp_body_len);

            data += udp_head_len;   // 将data指向UDP数据部分

            //strncpy(TUMessage, data, udp_body_len);
            break;
        }
        default: {  // 如果有其他可能情况，给出提示
            DEBUG("Other TranLayer proto=%d, with IPPROTO_TCP=%d, with IPPROTO_UDP=%d\n", iph->protocol, IPPROTO_TCP,
                  IPPROTO_UDP);
            return NF_ACCEPT;
        }
    }

    //DEBUG("data:%s", TUMessage);
    DEBUG("data:%s", data);

    // 检查数据部分合法性
    if (isLegal(data)) {
        DEBUG("data is legal");
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
    } else {
        DEBUG("data is illegal");
    }

    return NF_ACCEPT;
}
