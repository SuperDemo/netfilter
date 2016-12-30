
/* socket udp客户端*/
#include<stdio.h>
#include<stdlib.h>  
#include<string.h>  
#include<sys/socket.h>  
#include<sys/types.h>  
#include<unistd.h>  
#include<netinet/in.h>  
#include <errno.h>
#include <time.h>

#define SERVER_PORT 6666

int main() {
    int clientSocket;   // 客户端套接字
    struct sockaddr_in serverAddr;  // 服务端地址
    int addrlen=sizeof(serverAddr);
    char* alphabet = "abcdefghijklmnopqrstuvwxyz";
    char sendbuf[100];
    int index;
    clock_t start, end;

    // 客户端套接字
    if (clientSocket = socket(AF_INET, SOCK_DGRAM, 0) < 0){
        printf("socket error:%s\n",strerror(errno));
        return -1;
    }

    // 配置服务端套接字
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(SERVER_PORT);



    // 循环与服务端通信
    int count = 0;

    start = clock();
    while(1) {
        index = count % 26 + 1;     // 1--26
        strncpy(sendbuf, alphabet, index);  // a, ab, ..., a-z

        if (sendto(clientSocket, sendbuf, 100, 0, (struct sockaddr *)&serverAddr, addrlen) < 0) {
            printf("sendto error:%s\n",strerror(errno));
            continue;
        }
        count++;

        if (count % 1000 == 0){
            sendbuf[index] = '\0';
            printf("count=%8d, send data is %s\n", count, sendbuf);
        }

        if (count == 10000000){
            send(clientSocket, "quit", strlen("quit"), 0);
            break;
        }

//        printf("waiting for server......\n");
//        n=recvfrom(sockfd,recvline,200,0,(struct sockaddr *)&addr_ser,&addrlen);
//        if(n==-1)
//        {
//            printf("recvfrom error:%s\n",strerror(errno));
//            return -1;
//        }
//        recvline[n]='\0';
//        printf("recv data is:%s\n",recvline);
    }
    end = clock();

    printf("total time:%lf\n",(double)(end-start)/CLOCKS_PER_SEC);
    printf("total count=%8d\n", count);

    close(clientSocket);

    return 0;
}  