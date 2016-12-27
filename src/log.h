/**
 * log主要用来提供日志宏定义，便于统一修改日志等级
 */

#ifdef LOGUSER    // 如果定义此宏，表示接下来宏展开是将消息使用netlink发往用户态的

#include <linux/string.h>
extern char mymessagebuf[1000];  // 放置缓冲区声明

#define DEBUG(...) sprintf(mymessagebuf, __VA_ARGS__);sendMsgNetlink(mymessagebuf);
#define INFO(...) sprintf(mymessagebuf, __VA_ARGS__);sendMsgNetlink(mymessagebuf);
#define WARNING(...) sprintf(mymessagebuf, __VA_ARGS__);sendMsgNetlink(mymessagebuf);
#define ERROR(...) sprintf(mymessagebuf, __VA_ARGS__);sendMsgNetlink(mymessagebuf);

#else   // 如果没有定义LOGUSER，则表示接下来宏展开是将消息打印在内核中的

#define DEBUG(...) printk(KERN_DEBUG "DEBUG:" __VA_ARGS__)
#define INFO(...) printk(KERN_INFO "INFO:" __VA_ARGS__)
#define WARNING(...) printk(KERN_WARNING "WARING:" __VA_ARGS__)
#define ERROR(...) printk(KERN_EMERG "ERROR:" __VA_ARGS__)

#endif