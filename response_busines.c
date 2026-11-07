#include<epoll_threadpool_server.h>

void * response_busines(void * arg){
    int sockfd = *(int *)arg;
    char buf[1500];
	int recvlen;
	time_t tp;
	char time_buf[1024];
	char *err_msg = "please try again\n";
	bzero(buf, sizeof(buf));
	while(1)
	{
	recvlen = recv(sockfd, buf, sizeof(buf)-1, MSG_DONTWAIT);
	if(recvlen > 0)
	{
	buf[recvlen] = '\0';
	char *p = strstr(buf, "\n");
	if(p) *p = '\0';
	p = strstr(buf, "\r");
	if(p) *p = '\0';
	if(strcmp(buf, "localtime") == 0)
	{
	tp = time(NULL);
	bzero(time_buf, sizeof(time_buf));
	ctime_r(&tp, time_buf);
	send(sockfd, time_buf, strlen(time_buf), MSG_NOSIGNAL);
	}
	else
	{
	send(sockfd, err_msg, strlen(err_msg), MSG_NOSIGNAL);
	}
	bzero(buf, sizeof(buf));
	}
	else if(recvlen == 0)
	{
	printf("client fd:%d exit, remove from epoll\n", sockfd);
	epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
	close(sockfd);
	return NULL;
	}
	else
	{
	if(errno == EAGAIN || errno == EWOULDBLOCK)
	{
	break;
	}
	perror("response_busines recv failed");
	epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
	close(sockfd);
	return NULL;
	}
	}
	sleep(10);
	return NULL;
}
