#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>
#include <errno.h>

#include "../src/netLink.h"

#define MAXMSG 1000

struct netlink_message {
    struct nlmsghdr hdr;    //netlink message header
    char my_msg[MAXMSG];    //message body
} send_buf, recv_buf;

int main(int argc, char *argv[]) {

    // 创建客户端原始套接字，协议为NETLINK_TEST
    int sock_fd;
    if ((sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_TEST)) < 0) {
        perror("can't create netlink socket!\n");
        return 1;
    }

    // 创建目标地址数据结构
    struct sockaddr_nl dest_addr;
    int dest_addr_len = sizeof(dest_addr);  //该数据结构长度
    memset(&dest_addr, 0, sizeof(dest_addr));   //清空
    dest_addr.nl_family = AF_NETLINK;   // 设置协议簇
    //dest_addr.nl_pad = 0;
    dest_addr.nl_pid = 0;
    dest_addr.nl_groups = 0;



    // 设置netlink消息
    send_buf.hdr.nlmsg_len = sizeof(struct netlink_message);
    send_buf.hdr.nlmsg_flags = 0;
    send_buf.hdr.nlmsg_type = NLMSG_NOOP;
    send_buf.hdr.nlmsg_pid = getpid();
    strcpy(send_buf.my_msg, "testclient");

    //send message
    if (sendto(sock_fd, &send_buf, sizeof(send_buf), 0, (struct sockaddr *) &dest_addr, sizeof(dest_addr)) < 0) {
        printf("send message to kernel failed!\n");
        return 1;
    }

    //recv message
    while (1) {
        // printf("waiting message from kernel!\n");
        if (recvfrom(sock_fd, &recv_buf, sizeof(recv_buf), 0, (struct sockaddr *) &dest_addr, &dest_addr_len) < 0) {
            printf("recv message from kernel failed!\n");
        } else {
            printf("Get messages:%s\n", recv_buf.my_msg);
        }
    }

    close(sock_fd);
    return 0;
}
