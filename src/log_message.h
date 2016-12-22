
#include <linux/string.h>
extern char mymessagebuf[100];  // 放置缓冲区声明

#define DEBUG(...) sprintf(mymessagebuf, __VA_ARGS__);sendMsgNetlink(mymessagebuf);
#define INFO(...) sprintf(mymessagebuf, __VA_ARGS__);sendMsgNetlink(mymessagebuf);
//#define DEBUG(...) printk(KERN_DEBUG "DEBUG:" __VA_ARGS__)
//#define INFO(...) printk(KERN_INFO "INFO:" __VA_ARGS__)
#define WARNING(...) printk(KERN_EMERG "WARING:" __VA_ARGS__)
#define ERROR(...) printk(KERN_EMERG "ERROR:" __VA_ARGS__)
