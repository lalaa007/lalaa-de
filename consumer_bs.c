#include<epoll_threadpool_server.h>

void * consumer_bs(void * arg){
	//消费者线程函数，持续获取并执行任务
	pool_t * pt = (pool_t * )arg;
	task_t tmp;
	printf("cusomer 0x%x,waiting....\n",(unsigned int)pthread_self());
	while(pt->thread_shutdown){
		pthread_mutex_lock(&pt->lock);
		while(pt->cur==0){
			pthread_cond_wait(&pt->NOT_EMPTY,&pt->lock);
			if(!pt->thread_shutdown){
				//开关被关闭
				--pt->thread_alive;
				pthread_mutex_unlock(&pt->lock);
				printf("cusomer 0x%x shutdown %d,thread exit\n",(unsigned int)pthread_self(),pt->thread_shutdown);
				pthread_exit(NULL);
			}
			//关于缩减
			if(pt->thread_exitcode>0){
				--pt -> thread_alive;
				--pt->thread_exitcode;
				pthread_mutex_unlock(&pt->lock);
				printf("cusomer 0x%x kill.\n",(unsigned int)pthread_self());
				pthread_exit(NULL);
			}
		}
		//获取并执行任务
		tmp.busines = pt->list[pt->reat].busines;
		tmp.arg = pt->list[pt->reat].arg;
		--(pt->cur);
		pt->reat = (pt->reat + 1)%pt->max;
		++(pt->thread_busy);
		pthread_mutex_unlock(&pt->lock);
		pthread_cond_signal(&pt->NOT_FULL);
		//执行任务
		tmp.busines(tmp.arg);
		pthread_mutex_lock(&pt->lock);
		--(pt->thread_busy);
		pthread_mutex_unlock(&pt->lock);
	}
	printf("cusomer 0x%x,thread shutdown %d ,exit\n",(unsigned int)pthread_self(),pt->thread_shutdown);
	return NULL;
}
