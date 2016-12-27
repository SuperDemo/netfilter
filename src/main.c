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

MODULE_LICENSE("GPL");

char mymessagebuf[100];     // 缓冲区定义

static int __init init(void){
    // 插入模块时

    char *readFileData;

    INFO("insert netfilter module to kernel!\n");

    createNetlink();    // 初始化netlink模块

    // 从配置文件中读取配置
    readFileData = readConf();
    if (!readFileData) {
        WARNING("cannot readConf!\n");
        return 1;
    } else {
        DEBUG("readConf: %s\n", readFileData);
    }

    // 解析配置
    parseConf(readFileData);

    // 初始化netfilter
    //initNetFilter();

    return 0;
}

static void __exit fini(void){
    // 移除模块时

    INFO("remove netfilter module from kernel!\n");

    deleteNetlink();    // 释放netlink

    //releaseNetFilter(); //释放netfilter钩子

}

module_init(init);  // 模块入口，插入模块后调用绑定函数
module_exit(fini);  // 模块出口，插入模块后调用绑定函数