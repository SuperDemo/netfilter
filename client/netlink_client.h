
#ifndef __NETLINK_HEADER__
#define __NETLINK_HEADER__

#define NETLINK_TEST 20 // 自定义的netlink协议号
#define NETLINK_TEST_CONNECT 12  // 自定义的netlink客户端发送连接请求时type
#define NETLINK_TEST_DISCONNECT 23   // 自定义的netlink客户端发送断开连接请求时type
#define NETLINK_TEST_COMMAND 45  // 自定义的netlink客户端发送的指令
#define NETLINK_TEST_REPLY  56   // 自定义的netlink内核回复的消息类型

#define MAXMSG 1000

struct packet_info{
    // 可更改的消息结构体，表示内核与用户态通信的内容，暂定为字符串内容
    //int size;
    char data[MAXMSG];
};

#endif