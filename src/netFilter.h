


#define MIN_SIZE 96 //符合给定schema的最小数据包长度

// 初始化netfilter
//int initNetFilter();

// 钩子函数声明
unsigned int hook_func(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in,
                       const struct net_device *out, int (*okfn)(struct sk_buff *));