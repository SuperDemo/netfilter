#include <linux/init.h>  
#include <linux/module.h>  
#include <linux/timer.h>  
#include <linux/time.h>  
#include <linux/types.h>  
#include <net/sock.h>  
//#include <net/netlink.h>

#include <linux/netlink.h>
#include <linux/kernel.h>
  
#define NETLINK_TEST 20 
#define MAX_MSGSIZE 1024  
MODULE_AUTHOR("shaomai");  
MODULE_LICENSE("GPL");

int stringlength(char *s);  
void sendnlmsg(char * message,int dstPID); 
static int pid; 
static int pid1; // jiemian pid  
static int pid2; // user process pid  
static int err;  
static struct sock *nl_sk = NULL;  
int handleFlag = 0; 
static int j;
EXPORT_SYMBOL(handleFlag); 
EXPORT_SYMBOL(pid1);
EXPORT_SYMBOL(pid2);
  
void sendnlmsg(char *message,int dstPID)  
{  
    struct sk_buff *skb_1;  
    struct nlmsghdr *nlh;  
    int len = NLMSG_SPACE(MAX_MSGSIZE);  
    int slen = 0;  
    if(!message || !nl_sk)  
    {  
        return ;  
    }  
      
    skb_1 = alloc_skb(len,GFP_KERNEL);  
    if(!skb_1)  
    {  
        printk(KERN_ERR "my_net_link:alloc_skb_1 error\n");  
    }  
      
    slen = stringlength(message);  
    nlh = nlmsg_put(skb_1,0,0,0,MAX_MSGSIZE,0);  
  
    NETLINK_CB(skb_1).pid = 0;  
    NETLINK_CB(skb_1).dst_group = 0;  
  
    message[slen]= '\0';  
    memcpy(NLMSG_DATA(nlh),message,slen+1);  
    printk("my_net_link:send message '%s'.\n",(char *)NLMSG_DATA(nlh));  
    //printk("pid is: %d \n",pid);
    netlink_unicast(nl_sk,skb_1,dstPID,MSG_DONTWAIT);  
}  
EXPORT_SYMBOL(sendnlmsg);

int stringlength(char *s)  
{  
    int slen = 0;  
  
    for(; *s; s++){  
        slen++;  
    }  
  
    return slen;  
}  
EXPORT_SYMBOL(stringlength);

void nl_data_ready(struct sk_buff *__skb)  
{   
    struct sk_buff *skb;  
    struct nlmsghdr *nlh;  
    char str[100];  
    struct completion cmpl;  
    //int i=2;  
    skb = skb_get (__skb);  
      
    if(skb->len >= NLMSG_SPACE(0))
    // if(1)  
    {  
        nlh = nlmsg_hdr(skb);  
  
        memcpy(str, NLMSG_DATA(nlh), sizeof(str));  
        printk("try.c  Message received:%s\n",str) ;  
       //  printk("Message") ;  
        pid = nlh->nlmsg_pid; 
        printk("try.c  pid is: %d \n",pid); 
        if(1){     //if(j)  //dai
            pid2=pid;
            // if(pid1==pid2){
            //     sendnlmsg("1 from kernel!",pid1);
            // }else{
            //     sendnlmsg("2 from kernel!",pid2);
            //     sendnlmsg("3 from kernel!",pid1);
            // }
        }else{
            pid1=pid;
            //sendnlmsg("1 from kernel!",pid1);
        }
        // while(i--)  
        // {  
        //     init_completion(&cmpl);  
        //     wait_for_completion_timeout(&cmpl,3 * HZ);  
        // sendnlmsg("I am from kernel!");  
        // } 
	 if(str){
	  printk("The message from user in try.c  :: %s",str);
	}
        if(!strcmp(str,"pass")){
            printk("this is pass\n");
            handleFlag = 1;  
        }else if(!strcmp(str,"alert")){
            handleFlag = 2;  
        }else if(!strcmp(str,"drop")){
            handleFlag = 3;  
        }
    kfree_skb(skb);  
    }  
    j++;
}  
  
// Initialize netlink  
  
static int __init netlink_init(void){
  
    j=0;
    //kernel 2.6
    //nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST, 1, nl_data_ready, NULL, THIS_MODULE);  //nl_data_ready比较重要，它有点类似于以前的初始化接收函数，但现在我们不需要自己调用调度的方法，让内核进程阻塞以便等待消息到来了。只要声明一个nl_data_ready函数，这里将其地址赋给此参数，netlink内部会自动处理消息到来事件，一旦收到来自用户空间的匹配消息，将自动调用nl_data_ready函数进行处理。

    //kernel 3.10
    struct netlink_kernel_cfg cfg;
    cfg.groups = 1; //multi broadcat?
    cfg.flags = 0;
    cfg.skb = nl_data_ready;    //bound function
    cfg.cb_mutex = NULL;
    cfg.group = 0;  //?


    nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST, )
  
    if(!nl_sk){  
        printk(KERN_ERR "my_net_link: create netlink socket error.\n");  
        return 1;  
    }  
    printk("成功加载netlink模块！\n");
    printk("my_net_link: create netlink socket ok.\n");  

    return 0;  
}  
  
static void netlink_exit(void)  
{  
    if(nl_sk != NULL){  
        netlink_kernel_release(nl_sk);  
    }  
  
    printk("my_net_link: self module exited\n");  
}  
  
module_init(netlink_init);  
module_exit(netlink_exit);  
  
 
