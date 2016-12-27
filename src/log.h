/**
 * log主要用来提供日志宏定义，便于统一修改日志等级
 */

#define DEBUG(...) printk(KERN_INFO "DEBUG:" __VA_ARGS__)
#define INFO(...) printk(KERN_INFO "INFO:" __VA_ARGS__)
#define WARNING(...) printk(KERN_WARNING "WARING:" __VA_ARGS__)
#define ERROR(...) printk(KERN_EMERG "ERROR:" __VA_ARGS__)