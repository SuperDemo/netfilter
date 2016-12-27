

// 初始化netfilter
int initNetFilter(void);

// 钩子函数声明
unsigned int hook_func(unsigned int hooknum, struct sk_buff *skb, const struct net_device *in,
                       const struct net_device *out, int (*okfn)(struct sk_buff *));

// 释放netfilter钩子
int releaseNetFilter(void);