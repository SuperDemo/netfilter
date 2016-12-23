
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

#define SERVER_PORT 5555

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char rawbuf[200];
    char sendbuf[200];
    char recvbuf[200];
    int iDataNum;
    int i;
    int j;

    // 创建客户端套接字
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("create client socket failed!");
        return 1;
    }

    // 配置客户端套接字
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 连接服务端
    if (connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect to server failed!");
        return 1;
    }

    printf("connect with destination host...\n");

    // 循环与服务端通信
    strcpy(rawbuf, "abcdefghijklmnopqrstuvwxyz");
    for (i = 0; i < 100000; i++) {
        //printf("Input your word:>");
        //scanf("%s", sendbuf);
        //printf("\n");
        j = i % 26;
        memset(sendbuf, 0, sizeof(sendbuf));
        strncpy(sendbuf, rawbuf, j + 1);

        send(clientSocket, sendbuf, strlen(sendbuf), 0);
        if (strcmp(sendbuf, "quit") == 0)
            break;
        usleep(100000);
        //iDataNum = recv(clientSocket, recvbuf, 200, 0);
        //recvbuf[iDataNum] = '\0';
        //printf("recv data of my world is: %s\n", recvbuf);
    }
    close(clientSocket);
    return 0;
}