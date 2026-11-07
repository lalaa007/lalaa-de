#include<epoll_threadpool_server.h>

int epoll_listen(int server_sock,pool_t * pt){
	task_t ts;
	int readynum;
	struct epoll_event ready_array[100000];
	int i;
	printf("epoll_listen start...\n");
	while(pt->thread_shutdown){
		if((readynum = epoll_wait(epfd,ready_array,100000,-1))==-1){
			perror("epoll_wait call failed");
			exit(0);
		}
		i=0;
		while(readynum){
			if(ready_array[i].data.fd ==server_sock){
				ts.busines = accept_busines;
				ts.arg = (void *)&server_sock;
				producer_add_task(pt,ts);//添加任务
				printf("producer add accept busines.\n");
			}
			else{
				ts.busines = response_busines;
				ts.arg = (void *)&ready_array[i].data.fd;
				producer_add_task(pt,ts);//添加任务
				printf("producer add response busines.\n");
			}
			++i;
			--readynum;
		}
	}
	return 0;
}
