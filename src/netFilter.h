

char direction[50];
char titlecontent[50];
char content_flag[20];
char isapi[50];
char content[50];
char action[50];
char sourceip[50];
char targetip[50];

#define MIN_SIZE 96 //符合给定schema的最小数据包长度

struct nf_hook_ops nfho_single;  // netfilter钩子

// 钩子函数声明
unsigned int hook_func(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in,
                       const struct net_device *out, int (*okfn)(struct sk_buff *));