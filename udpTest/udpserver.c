
/*socket udp服务器端*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>

#define SERVER_PORT 6666

int main()
{
    int serverSocket;   // 服务端套接字
    struct sockaddr_in serverAddr;  // 服务端地址
    struct sockaddr_in clientAddr;  // 客户端地址
    int n;  // 接收长度
    int addrlen = sizeof(struct sockaddr);    // 地址长度，用于recvfrom函数
    int count = 0;  // 记录接收到的包的数量
    clock_t start, end; // 时钟，用于记录运行时间

    char recvline[200],sendline[200];

    // 创建服务端套接字
    if((serverSocket = socket(AF_INET,SOCK_DGRAM,0)) < 0){
        printf("socket error:%s\n",strerror(errno));
        return -1;
    }

    // 配置服务端地址
    bzero(&serverAddr, sizeof(serverAddr));    // 先清空
    serverAddr.sin_family=AF_INET;     // 协议簇
    serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);      // ip
    serverAddr.sin_port=htons(SERVER_PORT);      // 端口

    // 将套接字与地址绑定
    if (bind(serverSocket,(struct sockaddr *)&serverAddr,sizeof(serverAddr)) < 0){
        printf("bind error:%s\n",strerror(errno));
        return -1;
    }

    start = clock();    // 记录循环开始前时间

    // 循环与客户端通信
    while(1)
    {
        printf("waiting for client......\n");
        n = recvfrom(serverSocket, recvline, 1024, 0, (struct sockaddr *)&clientAddr, &addrlen);
        if(n < 0){
            printf("recvfrom error:%s\n",strerror(errno));
            return -1;
        }
        recvline[n]='\0';
        count++;

        if (count % 1000 == 0){
            printf("count = %8d, recv data is %s\n", count, recvline);
        }

        if (strcmp(recvline, "quit") == 0)
            break;

        // 将接收到的字符串再发回给客户端
//        n = sendto(serverSocket, recvline, 200, 0, (struct sockaddr *)&clientAddr, addrlen);
//        if(n < 0){
//            printf("sendto error:%s\n",strerror(errno));
//            return -1;
//        }
    }

    end = clock();  // 循环结束时记录时间

    printf("total time:%lf\n",(double)(end-start)/CLOCKS_PER_SEC);
    printf("total count=%8d\n", count);

    return 0;
} 