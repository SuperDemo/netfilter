    #include<stdio.h>  
    #include<stdlib.h>  
     #include <unistd.h>
    #include<string.h>  
    #include<sys/socket.h>  
    #include<netinet/in.h>  
    #include<arpa/inet.h>  
    #include<netdb.h>  
    #include<errno.h>  
    #include<sys/types.h>  
    int port=8888;

    #define BUFFER_SIZE 1024

    int count = 3;
    static int ievent = 0;
    char *buffer[6];    //????



    char *eventbuffer[11] = {"A2","A3","A5","A2","A3","A5","B","A2","A3","B","A5"}; ;
    char *head  = "<?xml version='1.0' encoding='gb2312' ?>";;
    char *countbuf[6]  = {"0","1","2","3","4","5"};;
    char data[1024];

    ///定义sockfd
    int sockfd;


      struct sockaddr_in adr_srvr;  

      int z;  

      char testbuff[1024]  = "<?xml version='1.0' encoding='gb2312' ?><note id='39'><title>ProblemAck</title><message>A3</message></note>";

    void func(char *buff)
     {
	  sleep(1);
	  ievent = (ievent++)%11;
        strcpy(data ,head);
        strcat(data, "<note id='");
        strcat(data, countbuf[count%6]);
        strcat(data,"'><title>");
        strcat(data,buffer[count]);
        strcat(data,"</title><message>");
        strcat(data,eventbuffer[ievent%11]);
        strcat(data,"</message></note>");
     	  printf("send message ::%s.\n",data);
        z=sendto(sockfd,data,sizeof(data),0,(struct sockaddr *)&adr_srvr,sizeof(adr_srvr));  
        if(z<0){  
           perror("recvfrom error");  
           exit(1);  
		}
    }


    void test1()
     {
        printf("test1\n");
        func("test1");
     }

    void test2()
     {
        printf("test2\n");
        func("test2");
        test1();
     }

    void test3()
     {
        printf("test3\n");
        func("test3");
        test2();
     }

    void func1()
     {
        printf("func1\n");
        func("func1");
     }

    void func2()
     {
        printf("func2\n");
        func("func2");
        func1();
     }


    void func3()
    {
        printf("func3\n");
        func("func3");
        func2();
    }





    int main(int argc,char *argv[]){


    	buffer[0] = "PollingData";
    	buffer[1] = "DataUpdate";

    	buffer[2] = "AlarmUpdate";
    	buffer[3] = "ProblemAck";

    	buffer[4] = "MdControl";
    	buffer[5] = "MdAction";

   	//data = (char *)malloc(1024);
    	data[0]=0;

      int i=0;  

      //char buf[80],str1[80];  
      //struct hostent *host;  

     // if(argc<2){  
      //  fprintf(stderr,"please enter the server's hostname!\n");  
    //    exit(1);  
 //     }

      //if((host=gethostbyname(argv[1]))==NULL){  
       // herror("gethostbyname error!");  
      //  exit(1);  
   //   }  


      //FILE *fp;  
      //printf("open file....\n");  
      //fp=fopen("liu","r");  
     // if(fp==NULL){  
     //   perror("failed to open file");  
    //    exit(1);  
   //   }  
      printf("connecting server....\n");  
      adr_srvr.sin_family=AF_INET;  
      adr_srvr.sin_port=htons(port);  
      //adr_srvr.sin_addr=*((struct in_addr *)host->h_addr);

        adr_srvr.sin_addr.s_addr = inet_addr("127.0.0.1");  ///服务器ip

      bzero(&(adr_srvr.sin_zero),8);  
      sockfd=socket(AF_INET,SOCK_DGRAM,0);  
      if(sockfd==-1){  
        perror("socket error!");  
        exit(1);  
      }  
      printf("send file ...\n");  
     /// char buffer[1024]  = "<?xml version='1.0' encoding='gb2312' ?><note id='39'><title>ProblemAck</title><message>A3</message></note>";
   ///   int count = 10000;
   ///   while(count-->0){
    ///     sleep(1);
         //fgets(str1,1024,buffer);  
         //printf("%d:%s",i,str1);  
         //sprintf(buf,"%d:%s",i,buffer);  
   ///      z=sendto(sockfd,buffer,sizeof(buffer),0,(struct sockaddr *)&adr_srvr,sizeof(adr_srvr));  
   ///      if(z<0){  
   ///        perror("recvfrom error");  
    ///       exit(1);  
     ///    }  
     // }  
      //printf("send ....\n");
      //sprintf(buf,"stop\n");
      //z=sendto(sockfd,buf,sizeof(buf),0,(struct sockaddr *)&adr_srvr,sizeof(adr_srvr));
      //if(z<0){
       // perror("sendto error");
       // exit(1);
      //}
    //  fclose(fp);  

    int var=1,j=1;
    printf("main\n");
    while(1){
        //usleep(500*1000);
        test1();
        test2();
        test3();
        func3();
        //if(j%11 == 0)
        //{ var++;}
        //i++;

    }


      close(sockfd);  
    
	return 0;  
}  

