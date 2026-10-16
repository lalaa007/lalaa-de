#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main(void)
{
	pid_t pid;
    int i;
	printf("父进程 pid = %d, ppid=%d, pgid=%d, sid=%d\n",
		getpid(), getppid(), getpgrp(), getsid(getpid()));
	// 循环创建3个子进程：一父三子
	for(i = 0; i < 3; i++)
	{
        pid = fork();
		if(pid < 0)												       
		{
			perror("fork error");
			exit(1);
		}
		if(pid == 0)
		{
			// =========子进程：创建新会话，脱离控制终端=========
			setsid();  
			printf("【子进程%d】pid=%d, ppid=%d, pgid=%d, sid=%d\n",i+1, getpid(), getppid(), getpgrp(), getsid(getpid()));																				
			// 子进程死循环，保持不退出，方便我们观察
			while(1)
			{	
				sleep(2);
				printf("子进程%d still alive! pid=%d\n",i+1,getpid());			
			}
			exit(0);
			}
			// pid>0：父进程，继续循环创建下一个子进程
		}
		// 父进程等待，父进程本身不做setsid
		while(1)
		{
			sleep(3);
			printf("父进程 running pid=%d\n", getpid());
		}
		return 0;
}
