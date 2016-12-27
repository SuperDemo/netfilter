/**
 * netLink运行在内核，用来向用户态进程发送消息并接收用户态消息
 */

#define NETLINK_TEST 20 // 自定义的netlink协议号
#define NETLINK_TEST_CONNECT 123  // 自定义的netlink客户端发送连接请求时type
#define NETLINK_TEST_DISCONNECT 234   // 自定义的netlink客户端发送断开连接请求时type
#define NETLINK_TEST_COMMAND 456  // 自定义的netlink客户端发送的指令
#define NETLINK_TEST_REPLY  567   // 自定义的netlink内核回复的消息类型

int createNetlink(void);

int sendMsgNetlink(char *message);

//void recvMsgNetlink(void);

int deleteNetlink(void);