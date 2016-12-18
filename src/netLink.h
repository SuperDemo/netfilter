/**
 * netLink运行在内核，用来向用户态进程发送消息并接收用户态消息
 */

int createNetlink(void);

void sendMsgNetlink(void);

void recvMsgNetlink(void);

void deleteNetlink(void);