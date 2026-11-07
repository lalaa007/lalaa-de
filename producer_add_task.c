#include<epoll_threadpool_server.h>

int producer_add_task(pool_t * pt,task_t ts){
	/*调用一次添加一个任务*/
	if(pt->thread_shutdown){
		pthread_mutex_lock(&pt->lock);
		while(pt->cur == pt->max){
			pthread_cond_wait(&pt->NOT_FULL,&pt->lock);
			if(!pt->thread_shutdown){
				pthread_mutex_unlock(&pt->lock);
				printf("shutdown is %d ,add task failed return -1\n",pt->thread_shutdown);
				return -1;
			}
		}
		//add task
		pt->list[pt->front].busines = ts.busines;
		pt->list[pt->front].arg = ts.arg;
		++pt->cur;
		pt->front = (pt->front +1 )%pt->max;//环形偏移
		pthread_mutex_unlock(&pt->lock);
		pthread_cond_signal(&pt->NOT_EMPTY);//唤醒消费者
	}
	return 0;
}
