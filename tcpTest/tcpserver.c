
/*socket tcp服务器端*/
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define SERVER_PORT 5555

int main() {
    int serverSocket;
    struct sockaddr_in server_addr;
    struct sockaddr_in clientAddr;
    int addr_len = sizeof(clientAddr);
    int client;
    char buffer[2000];
    int iDataNum;
    unsigned int value = 0x1;

    // 创建服务端套接字
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("create socket failed!");
        return 1;
    }

    // 设置地址复用
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (void *) &value, sizeof(value));

    // 配置服务端地址
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 将套接字与地址绑定
    if (bind(serverSocket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind socket failed!");
        return 1;
    }

    // 服务端开始监听，最大连接数为5
    if (listen(serverSocket, 5) < 0) {
        perror("server start listening failed!");
        return 1;
    }

    // 循环接收客户端数据
    printf("Listening on port: %d\n", SERVER_PORT);

    // 接收客户端请求，建立一个客户端套接字
    client = accept(serverSocket, (struct sockaddr *) &clientAddr, (socklen_t *) &addr_len);
    if (client < 0) {
        perror("accept from client failed!");
        return 1;
    }
    printf("\nrecv client data...n");
    printf("IP is %s\n", inet_ntoa(clientAddr.sin_addr));
    printf("Port is %d\n", htons(clientAddr.sin_port));

    // 循环接受当次连接客户端发送给服务端的数据
    while (1) {
        iDataNum = recv(client, buffer, 1024, 0);   // 单次接收1024字节
        if (iDataNum < 0) {
            perror("recv from client failed!");
            continue;
        }
        buffer[iDataNum] = '\0';    // 将接收到的字符串收尾

        printf("%d recv data is %s\n", iDataNum, buffer);

        if (strcmp(buffer, "quit") == 0)
            break;

        // 将接收到的字符串再发回给客户端
        send(client, buffer, iDataNum, 0);
    }
    return 0;
}