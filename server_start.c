#include<epoll_threadpool_server.h>

int server_start(int shutdown){
	int server_sock;
	server_sock = net_initializer(NULL,8080);
	epoll_initializer(server_sock);
	pool_t * pt = thread_pool_initializer(100,10,1000,shutdown);
	//开始监听
	epoll_listen(server_sock,pt);
}
