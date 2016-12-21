/**
 * log主要用来提供日志宏定义，便于统一修改日志等级
 */
#ifndef LOG
#define LOG

#include <linux/string.h>

//char messagebuf[100];

#define DEBUG(...) printk(KERN_EMERG "DEBUG:" __VA_ARGS__)
#define INFO(...) printk(KERN_EMERG "INFO:" __VA_ARGS__)
#define WARNING(...) printk(KERN_EMERG "WARING:" __VA_ARGS__)
#define ERROR(...) printk(KERN_EMERG "ERROR:" __VA_ARGS__)

//#define DEBUG(...) sendMsgNetlink("DEBUG:" __VA_ARGS__)
//#define DEBUG(...) sprintf(messagebuf, __VA_ARGS__);sendMsgNetlink(messagebuf);

#endif