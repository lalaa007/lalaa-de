#include<epoll_threadpool_server.h>

int if_thread_alive(pthread_t tid){
	int err;
	err=pthread_kill(tid,0);//测试线程是否存活
	if(err==ESRCH)
		return 0;//线程失效
	else
		return 1;//线程有效
}
