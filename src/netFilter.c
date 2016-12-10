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
//#include "alarmDetection.h"
#include "netFilter.h"
#include "dealConf.h"

static struct nf_hook_ops nfho_single;

////#define PORT 25
//#define SIZE 9 //dependence2中的给定事件集合buffer[]大小
#define MIN_SIZE 96 //符合给定schema的最小数据包长度
//
//
////钩子函数声明
unsigned int hook_func(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in,
                       const struct net_device *out, int (*okfn)(struct sk_buff *));

//unsigned static char *drop_ip1 = "\xC0\xA8\x00\x6A"; //匹配的数据包源IP地址, 此处为 10.109.253.24//xuniji10.108.167.12
//unsigned static char *drop_ip2 = "\xC0\xA8\x00\x69"; //匹配的数据包目的IP地址， 此处为 :10.108.165.88//benji
//unsigned static int count = 0; //记录过滤的数据包数目

char direction[50];
char titlecontent[50];
char content_flag[20];
char isapi[50];
char content[50];
char action[50];
char sourceip[50];
char targetip[50];
//
//
////alarmDetection
//extern struct form trace[4];
//extern char *buffer[9];
//
////dependence3
////extern void sendnlmsg(char *message);
//extern void sendnlmsg(char *message, int dstPID);
//
//extern int stringlength(char *s);
//
//extern int handleFlag;
//extern int pid1;
//extern int pid2;
//
//



//
////打印events链表中的所有元素
//void print_events(void) {
//
//    eventNode *temp = events;
//    int num = 0;
//
//    while (temp != NULL) {
//
//        ++num;
//
//        printk("%s  ", temp->data);
//        temp = temp->next;
//    }
//
//    printk("count=====%d\n", num);
//}
//
//


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

//    char message[50]; //记录message
//    char title[50]; //记录抽象事件==主题
//
//
//    int i = 0; //循环计数变量
//    struct completion cmpl;

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
            break;
        }
        case IPPROTO_UDP: {
            //获取udp头部，并计算其长度
            udphead = (struct udphdr *) data;
            udp_head_len = sizeof(struct udphdr);
            udp_body_len = udphead->len - udp_head_len;
            INFO("udp_head_len=%d,udp_body_len=%d\n", udp_head_len, udp_body_len);
            break;
        }
        default: {
            DEBUG("proto=%d,IPPROTO_TCP=%d,IPPROTO_UDP=%d\n", iph->protocol, IPPROTO_TCP, IPPROTO_UDP);
            return NF_ACCEPT;
        }
    }

    return NF_ACCEPT;
}
//    if (isLegal(data)) {
//        extractTitle(title, data);
//
//        printk("title is:%s\n", title);
//        //sendnlmsg(title,pid1);                             //dai
//        //对接收到到符合给定schema的数据包进行计数
//        ++count;
//
//        //当威胁事件集合不为空时，先判定事件是不是在集合中，如果在，则直接丢弃即可。
//        if (events->next != NULL) {
//
//            if (inSetOrNot(title) == 0) {
//
//                return NF_DROP;
//            }
//        }
//        if (!strcmp(isapi, "no")) {
//            if (strcmp(content, "")) {
//                if (!strcmp(content_flag, "all")) {
//                    if (strstr(data, content)) {
//                        if (!strcmp(action, "drop")) {
//                            // sendnlmsg(title);
//                            return NF_DROP;
//                        } else if (!strcmp(action, "alert")) {
//                            // sendnlmsg(title);
//                            return NF_ACCEPT;
//                        } else {
//                            return NF_ACCEPT;
//                        }
//                    }
//                }
//                    //暂时先匹配 message待加
//                else if (!strcmp(content_flag, "message")) {
//                    extractMessage(message, data);
//                    if (strstr(message, content)) {
//                        if (!strcmp(action, "drop")) {
//                            // sendnlmsg(title);
//                            return NF_DROP;
//                        } else if (!strcmp(action, "alert")) {
//                            // sendnlmsg(title);
//                            return NF_ACCEPT;
//                        } else {
//                            return NF_ACCEPT;
//                        }
//                    }
//                }
//            }
//            if (strcmp(titlecontent, "")) {
//                if (strstr(title, titlecontent)) {
//                    if (!strcmp(action, "drop")) {
//                        // sendnlmsg(title);
//                        return NF_DROP;
//                    } else if (!strcmp(action, "alert")) {
//                        // sendnlmsg(title);
//                        return NF_ACCEPT;
//                    } else {
//                        return NF_ACCEPT;
//                    }
//                }
//            }
//        } else if (!strcmp(isapi, "yes")) {
//            if (strstr(title, action)) {
//                // printk("handleFlag is :%d\n",handleFlag);
//                //if(pid1!=pid2){      //dai
//                sendnlmsg(data, pid2);
//                //}    //dai
//                init_completion(&cmpl);   //dai
//                wait_for_completion_timeout(&cmpl, 1 * HZ);   //dai
//                printk("handleFlag is :%d\n", handleFlag);
//                if (handleFlag == 1) {
//                    //sendnlmsg("special handle :let it pass!",pid1);
//                } else if (handleFlag == 2) {
//                    //sendnlmsg("special handle :warning!",pid1);
//                } else if (handleFlag == 3) {
//                    return NF_DROP;
//                }   //dai
//            }
//            handleFlag = 0;
//        }
//        //以下是流程匹配过程，算法原理很简单，请参考小论文
//        if (flag) {
//
//            //只要上一次匹配已经结束，那么下一次都有可能匹配轨迹trace[1]
//            if (strcmp(title, trace[1].next->data) == 0) {
//
//                matchNum = 1;
//                pre = trace[1].next;
//
//                if (pre->next) {
//                    flag = 0;
//                } else {
//                    flag = 1;
//                }
//
//                return NF_ACCEPT;
//            }
//
//            switch (matchNum) {
//                case 0: {
//
//                    //matchNum==0,即第一次进入流程，此时匹配轨迹trace[1]失败
//                    for (i = 0; i < SIZE; ++i) {
//
//                        if (strcmp(title, buffer[i]) == 0) {
//                            break;
//                        }
//                    }
//
//                    if (SIZE == i) {
//
//                        //此时，title不在给定事件集合中，添加到威胁事件集合中
//                        update_events(title);
//                        //print_events();
//                    }
//
//                    //return NF_DROP;
//                }
//                case 1: {
//                    //matchNum==1,即本次可能匹配trace[2]，此时trace[1]已经匹配失败
//                    if (strcmp(title, trace[2].next->data) == 0) {
//
//                        matchNum = 2;
//                        pre = trace[2].next;
//
//                        if (pre->next) {
//                            flag = 0;
//                        } else {
//                            flag = 1;
//                        }
//
//                        return NF_ACCEPT;
//                    }
//
//                    for (i = 0; i < SIZE; ++i) {
//
//                        if (strcmp(title, buffer[i]) == 0) {
//                            break;
//                        }
//                    }
//
//                    if (SIZE == i) {
//
//                        //此时，title不在给定事件集合中，添加到威胁事件集合中
//                        update_events(title);
//                    }
//
//                    //return NF_DROP;
//                }
//                case 2:
//                case 3: {
//                    //matchNum==2or3,即本次可能匹配trace[3]，此时trace[1]已经匹配失败
//                    if (strcmp(title, trace[3].next->data) == 0) {
//
//                        matchNum = 3;
//                        pre = trace[3].next;
//
//                        if (pre->next) {
//                            flag = 0;
//                        } else {
//                            flag = 1;
//                        }
//
//                        return NF_ACCEPT;
//                    }
//
//                    for (i = 0; i < SIZE; ++i) {
//
//                        if (strcmp(title, buffer[i]) == 0) {
//                            break;
//                        }
//                    }
//
//                    if (SIZE == i) {
//
//                        //此时，title不在给定事件集合中，添加到威胁事件集合中
//                        update_events(title);
//                    }
//
//                    //return NF_DROP;
//                }
//                default: {
//                    //matchNum!=0,1,2or3,说明数据包有问题，处理title主题
//                    for (i = 0; i < SIZE; ++i) {
//
//                        if (strcmp(title, buffer[i]) == 0) {
//                            break;
//                        }
//                    }
//
//                    if (SIZE == i) {
//
//                        //此时，title不在给定事件集合中，添加到威胁事件集合中
//                        update_events(title);
//                    }
//
//                    //return NF_DROP;
//                }
//            }
//        } else {
//            switch (matchNum) {
//
//                case 1:
//                case 2:
//                case 3: {
//
//                    if (strcmp(title, pre->next->data) == 0) {
//
//                        pre = pre->next;
//
//                        if (pre->next) {
//                            flag = 0;
//                        } else {
//                            flag = 1;
//                        }
//
//                        return NF_ACCEPT;
//
//                    }
//
//                    for (i = 0; i < SIZE; ++i) {
//
//                        if (strcmp(title, buffer[i]) == 0) {
//                            break;
//                        }
//                    }
//
//                    if (SIZE == i) {
//
//                        //此时，title不在给定事件集合中，添加到威胁事件集合中
//                        update_events(title);
//                    }
//
//                    //return NF_DROP;
//                }
//                default: {
//                    for (i = 0; i < SIZE; ++i) {
//
//                        if (strcmp(title, buffer[i]) == 0) {
//                            break;
//                        }
//                    }
//
//                    if (SIZE == i) {
//
//                        //此时，title不在给定事件集合中，添加到威胁事件集合中
//                        update_events(title);
//                    }
//
//                    //return NF_DROP;
//                }
//            }
//        }
//    }
//
//    return NF_ACCEPT;
//}

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
