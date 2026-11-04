#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<arpa/inet.h>

void *client_service_thread(void *arg)
{
//	    int client_sock = (int)(long long)arg;
        int *pfd = arg;
        int client_sock = *pfd;
        free(pfd);
        pthread_detach(pthread_self()); 
	    char buffer[4096];
		char *dfl_msg = "Please TryAgain.\n";
		time_t tp;
		char time_buf[1024];
		int len;
		//char response[4096];
	    //	char cip[16];
		
	//	bzero(cip,16);
	//	bzero(response,sizeof(response));
		
	    while((len=recv(client_sock,buffer,sizeof(buffer),0))>0){
	    //时间处理及响应
		buffer[strlen(buffer)-1]='\0';
		if(strcmp(buffer,"time")==0){
			bzero(time_buf,sizeof(time_buf));
			tp = time(NULL);
			ctime_r(&tp,time_buf);
			send(client_sock,time_buf,strlen(time_buf),MSG_NOSIGNAL);
		}
		else
			send(client_sock,dfl_msg,strlen(dfl_msg),MSG_NOSIGNAL);
		bzero(buffer,sizeof(buffer));
		}
		if(len==-1){
			perror("recv call failed");
		}
		else if(len==0){
			printf("client exit,close clientsock %d\n",client_sock);
		}
		close(client_sock);
		printf("thread %lu exit\n",pthread_self());
		return NULL;
}

int main(void){
	    //网络初始化
	    int server_sock;
		int client_sock;
		struct sockaddr_in server_addr;
		struct sockaddr_in client_addr;
		socklen_t addrlen;
		pthread_t tid;
		char cip[16];
		char response[4096];

		if((server_sock = socket(AF_INET,SOCK_STREAM,0))==-1){
			perror("sock call failed");
			exit(1);
		}
		bzero(&server_addr,sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(8080);
		server_addr.sin_addr.s_addr = INADDR_ANY;
		
		if((bind(server_sock,(struct sockaddr *)&server_addr,sizeof(server_addr)))==-1){
			perror("bind call failed");
			exit(1);
		}	
		listen(server_sock,128);
		printf("pthread server Running , main thread %lu Accepting..\n",pthread_self());

		while(1){
		addrlen=sizeof(client_addr);
		if((client_sock=accept(server_sock,(struct sockaddr *)&client_addr,&addrlen))==-1){
		perror("accept call failed\n");
		continue;
		}
		
		bzero(cip,16);
		bzero(response,sizeof(response));
		inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,cip,16);
		sprintf(response,"hi %s ,connect success.\n",cip);
		send(client_sock,response,strlen(response),MSG_NOSIGNAL);
		printf("server accept success , client ip %s client port %d\n",cip,ntohs(client_addr.sin_port));

		/*if(pthread_create(&tid,NULL,client_service_thread,(void*)(long long)client_sock) != 0)
		{
		perror("pthread create fail");
		close(client_sock);
		continue;
		}*/

		int *pfd = malloc(sizeof(int));
		*pfd = client_sock;
		if(pthread_create(&tid,NULL,client_service_thread,pfd) != 0)
		{
			    perror("pthread create fail");
				    close(client_sock);
					    free(pfd);
						    continue;
		}

	}
	close(server_sock);
	exit(0);
}


