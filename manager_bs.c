#include<epoll_threadpool_server.h>

void * manager_bs(void * arg){
	pool_t * pt = (pool_t *)arg;
	printf("manager 0x%xd Runing..\n",(unsigned int)pthread_self());
	int alive,busy,cur;
	int flag;//遍历可用的ctids 消费者tid数组
	int add;//记录扩容数量
	int err;
	while(pt->thread_shutdown){
		pthread_mutex_lock(&pt->lock);
		alive = pt->thread_alive;
		busy = pt->thread_busy;
		cur = pt->cur;
		pthread_mutex_unlock(&pt->lock);
		printf("alive %d busy %d cur %d B/A %.2f%% A/M %.2f%%\n",alive,busy,cur,(double)busy/alive * 100,(double)alive / pt->thread_max * 100);
		//判定扩容
		if((cur>alive-busy||(double)busy/alive*100>=70)&&alive + pt->thread_min<=pt->thread_max){
			for(flag=0,add=0;flag<pt->thread_max&&add<pt->thread_min;flag++){
				if(pt->ctids[flag]==0||!if_thread_alive(pt->ctids[flag])){
					if((err = pthread_create(&pt->ctids[flag],NULL,consumer_bs,(void *)pt))>0){
						printf("manager add consumer failed %s\n",strerror(err));
						exit(0);
					}
					add++;
					pthread_mutex_lock(&pt->lock);
					++(pt->thread_alive);
					pthread_mutex_unlock(&pt->lock);
				}
			}
		}
		//缩减
		if(busy * 2 <alive-busy){
			pt->thread_exitcode = pt->thread_min;//缩减值
			for(int i=0;i<pt->thread_min;i++)
				pthread_cond_signal(&pt->NOT_EMPTY);
		}
		sleep(1);
	}
	printf("manager,shutdown is %d ,thread exit\n",pt->thread_shutdown);
	return NULL;
}
