#include<epoll_threadpool_server.h>

int first_response(int client_sock,char * cip){
	char response[1024];
	bzero(response,sizeof(response));
	sprintf(response,"hi,%s connect select server successly.\n",cip);
	send(client_sock,response,strlen(response),MSG_NOSIGNAL);
	return 0;
}
