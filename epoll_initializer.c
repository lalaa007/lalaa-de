#include<epoll_threadpool_server.h>

int epoll_initializer(int sockfd){
	struct epoll_event node;
	node.data.fd = sockfd;
	node.events = EPOLLIN|EPOLLET;
	if((epfd = epoll_create(100000))==-1){
		perror("epoll_create failed");
		exit(0);
	}
	if((epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&node))==-1){
		perror("epoll initializer,epoll ctl call failed");
		exit(0);
	}
	printf("epoll initializer ,epoll ctl call failed");
	return 0;
}
