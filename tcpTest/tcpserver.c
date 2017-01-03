
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
#include <errno.h>
#include <time.h>

#define SERVER_PORT 21001

int main() {
    int serverSocket;   // 服务端套接字
    int clientSocket;   // 客户端套接字
    struct sockaddr_in serverAddr;  // 服务端地址
    struct sockaddr_in clientAddr;  // 客户端地址
    int addr_len = sizeof(clientAddr);  // 地址长度，用于accept函数
    char buffer[2000];  // 接收缓冲区
    int iDataNum;   // 接收长度
    unsigned int value = 0x1;   // 用于设置地址利用
    int count = 0;  // 记录接收到的包数量
    clock_t start, end;  // 时钟，用于记录运行时间

    // 创建服务端套接字
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket error:%s\n",strerror(errno));
        return -1;
    }

    // 设置地址复用
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (void *) &value, sizeof(value));

    // 配置服务端地址
    bzero(&serverAddr, sizeof(serverAddr));   // 清空
    serverAddr.sin_family = AF_INET;       // 协议簇
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);    // ip
    serverAddr.sin_port = htons(SERVER_PORT);      // 端口

    // 将套接字与地址绑定
    if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        printf("bind error:%s\n",strerror(errno));
        return -1;
    }

    // 服务端开始监听，最大连接数为5
    if (listen(serverSocket, 5) < 0) {
        printf("server start listening failed:%s\n",strerror(errno));
        return -1;
    }

    // 循环接收客户端数据
    printf("Listening on port: %d\n", SERVER_PORT);

    // 接收客户端请求，建立一个客户端套接字
    if((clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddr, (socklen_t *) &addr_len) < 0)){
        printf("accept from client failed:%s\n",strerror(errno));
        return -1;
    }

    printf("\nrecv client data...n");
    printf("IP is %s\n", inet_ntoa(clientAddr.sin_addr));
    printf("Port is %d\n", htons(clientAddr.sin_port));

    start = clock();    // 记录循环开始前时间

    // 循环接受当次连接客户端发送给服务端的数据
    while (1) {
        iDataNum = recv(clientSocket, buffer, 1024, 0);   // 单次最大接收1024字节
        if (iDataNum < 0) {
            printf("recv from client failed:%s\n",strerror(errno));
            continue;
        }
        buffer[iDataNum] = '\0';    // 将接收到的字符串收尾
        count++;

        if (count % 1000 == 0){
            printf("count=%8d, recv data is %s\n", count, buffer);
        }

        if (strcmp(buffer, "quit") == 0)
            break;

        // 将接收到的字符串再发回给客户端
        //send(clientSocket, buffer, iDataNum, 0);
    }

    end = clock();  // 循环结束时记录时间

    printf("total time:%lf\n",(double)(end-start)/CLOCKS_PER_SEC);
    printf("total count=%8d\n", count);

    close(clientSocket);
    close(serverSocket);

    return 0;
}