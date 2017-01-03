
/*socket tcp客户端*/
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
#include <time.h>
#define SERVER_PORT 21001

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char* alphabet = "abcdefghijklmnopqrstuvwxyz";
    char sendbuf[100];
    clock_t start, end;
    int index;
    int i;

    // 创建客户端套接字
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("create client socket failed!");
        return -1;
    }

    // 配置服务端套接字
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr("10.108.167.106");

    // 连接服务端
    if (connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect to server failed!");
        return -1;
    }

    printf("connect with destination host...\n");

    // 循环与服务端通信
    int count = 0;

    start = clock();
    for (i = 0; i < 100000; i++) {
        index = count % 26 + 1;     // 1--26
        strncpy(sendbuf, alphabet, index);  // a, ab, ..., a-z

        send(clientSocket, sendbuf, index, 0);

        count++;

        sendbuf[index] = '\0';
//        printf("count=%8d, send data is %s\n", count, sendbuf);
        usleep(100);

//        if (count % 10000 == 0){
//            sendbuf[index] = '\0';
//            printf("count=%8d, send data is %s\n", count, sendbuf);
//        }

        //iDataNum = recv(clientSocket, recvbuf, 200, 0);
        //recvbuf[iDataNum] = '\0';
        //printf("recv data of my world is: %s\n", recvbuf);
    }
    end = clock();

    printf("total time:%lf\n", (double)(end - start) / CLOCKS_PER_SEC);
    printf("total count=%8d\n", count);

    close(clientSocket);

    return 0;
}