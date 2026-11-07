#include<epoll_threadpool_server.h>
int net_initializer(char * ip,int port){
	struct sockaddr_in serveraddr;
	int sockfd;
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("socket call failed");
	}
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);//自定义端口>5000
	if(!ip)
		serveraddr.sin_addr.s_addr = INADDR_ANY;
	else
		inet_pton(AF_INET,ip,&serveraddr.sin_addr.s_addr);
	if((bind(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)))==-1)
		perror("bind call failed");
	if((listen(sockfd,128))==-1)
		perror("listen call failed");
	printf("net initializer success,server sock %d\n",sockfd);
	return sockfd;
}
