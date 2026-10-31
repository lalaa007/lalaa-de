#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#include<pthread.h>
#include<signal.h>
#include<sys/socket.h>
#include<arpa/inet.h>

int main(void){
	char buffer[4096];
	struct sockaddr_in destaddr;
	bzero(&destaddr,sizeof(destaddr));

	destaddr.sin_family=AF_INET;
	destaddr.sin_port=htons(8080);

	inet_pton(AF_INET,"127.0.0.1",&destaddr.sin_addr.s_addr);

	//create sock
	int sockfd;
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("socket create failed");
		exit(EXIT_FAILURE);
	}

	//sockfd = socket(AF_INET,SOCK_STREAM,0);
	//tcp connect
	if((connect(sockfd,(struct sockaddr *)&destaddr,sizeof(destaddr)))==-1){
		perror("connect call failed");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	printf("connect success\n");
	while(1){
		printf("please input:");
		fgets(buffer,sizeof(buffer),stdin);

		send(sockfd,buffer,strlen(buffer),0);

		bzero(buffer,sizeof(buffer));
		recv(sockfd,buffer,sizeof(buffer),0);
		printf("server response:%s\n",buffer);
	}
	/*bzero(buffer,sizeof(buffer));

	recv(sockfd,buffer,sizeof(buffer),0);
	printf("client recv response = %s",buffer);
	printf("client fin..\n");
	close(sockfd);*/
	close(sockfd);
	return 0;
}
