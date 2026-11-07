#include<epoll_threadpool_server.h>

void * accept_busines(void * arg){
    int server_sock = *(int *)arg;
    struct sockaddr_in clientaddr;
    socklen_t addrlen = sizeof(clientaddr);
    int clientfd;
	struct epoll_event node;
	char cip[16];
	while(1)
	{
	clientfd = accept(server_sock, (struct sockaddr *)&clientaddr, &addrlen);
	if(clientfd < 0)
	{
	if(errno == EAGAIN || errno == EWOULDBLOCK)
	{
	break;
	}
	perror("accept_busines accept failed");
	return NULL;
	}
	bzero(cip, sizeof(cip));
	inet_ntop(AF_INET, &clientaddr.sin_addr, cip, sizeof(cip));
	node.data.fd = clientfd;
	node.events = EPOLLIN | EPOLLET;
	if(epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &node) == -1)
	{
	perror("accept_busines epoll_ctl add client failed");
	close(clientfd);
	continue;
	}
	first_response(clientfd, cip);
	}
	return NULL;
}
