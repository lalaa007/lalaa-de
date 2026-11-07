#include<epoll_threadpool_server.h>

pool_t * thread_pool_initializer(int tmax,int tmin,int qmax,int shutdown){
	pool_t * pt = NULL;
	if((pt = (pool_t *)malloc(sizeof(pool_t)))==NULL){
		perror("malloc pt failed");
		exit(0);
	}
	pt->thread_shutdown = shutdown;
	pt->thread_max = tmax;
	pt->thread_min = tmin;
	pt->thread_busy = 0;
	pt->thread_alive = 0;
	pt->thread_exitcode = 0;

	pt->front = 0;
	pt->reat = 0;
	pt->cur = 0;
	pt->max = qmax;
	if((pt->list = (task_t *)malloc(sizeof(task_t)*qmax))==NULL){
		perror("malloc queue failed");
		exit(0);
	}
	if((pt->ctids = (pthread_t *)malloc(sizeof(pthread_t) * tmax))==NULL){
		perror("malloc ctids failed");
		exit(0);
	}
	bzero(pt->ctids,sizeof(pthread_t)*tmax);
	if(pthread_mutex_init(&pt->lock,NULL)!=0||pthread_cond_init(&pt->NOT_FULL,NULL)!=0||pthread_cond_init(&pt->NOT_EMPTY,NULL)!=0){
		printf("init lock or cond failed.\n");
		exit(0);
	}
	int err;
	for(int i=0;i<tmin;i++){
		if((err = pthread_create(&pt->ctids[i],NULL,consumer_bs,(void *)pt))>0){
			printf("create consumer failed\n");
			exit(0);
		}
		pthread_mutex_lock(&pt->lock);
		++(pt->thread_alive);
		pthread_mutex_unlock(&pt->lock);
	}
	if((err = pthread_create(&pt->mid,NULL,manager_bs,(void *)pt))>0){
		printf("create consumer failed\n");
		exit(0);
	}
	return pt;
}
