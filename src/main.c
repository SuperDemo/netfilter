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
#include<linux/string.h>

#include "log.h"
#include "dealConf.h"
#include "netFilter.h"
#include "netLink.h"

struct nf_hook_ops nfho_single;  // netfilter钩子

static int __init init(void){
    // 插入模块时

    char *readFileData;

    createNetlink();    // 初始化netlink模块

    INFO("加载netfilter模块！\n");

    // 从配置文件中读取配置
    readFileData = readConf();
    if (!readFileData) {
        WARNING("cannot readConf!\n");
        return 1;
    } else {
        DEBUG("readConf: %s\n", readFileData);
    }

    // 解析配置
    extract(direction, "direction", readFileData, 0, CONTENTMAXLEN);
    extract(content_flag, "content_flag", readFileData, 0, CONTENTMAXLEN);
    extract(content, "content", readFileData, 0, CONTENTMAXLEN);
    extract(sourceip, "sourceip", readFileData, 0, CONTENTMAXLEN);
    extract(targetip, "targetip", readFileData, 0, CONTENTMAXLEN);
    extract(action, "action", readFileData, 0, CONTENTMAXLEN);
    extract(titlecontent, "titlecontent", readFileData, 0, CONTENTMAXLEN);
    extract(isapi, "isapi", readFileData, 0, CONTENTMAXLEN);

    DEBUG("titlecontent:%s, "
                  "direction:%s, "
                  "sourceip:%s, "
                  "targetip:%s, "
                  "action:%s, "
                  "content_flag:%s, "
                  "content:%s, "
                  "isapi:%s\n",
          titlecontent, direction, sourceip, targetip, action, content_flag, content, isapi);

    return 0;

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
    return ;

    if (strcmp(direction, "=>") == 0) {   // 如果选择单向拦截
        DEBUG("单向拦截\n");
        nf_unregister_hook(&nfho_single);   // 卸载钩子
    } else if (strcmp(direction, "<=>") == 0) {     // 如果选择双向拦截
        DEBUG("双向拦截\n");
    }

}

module_init(init);  // 模块入口，插入模块后调用绑定函数
module_exit(fini);  // 模块出口，插入模块后调用绑定函数