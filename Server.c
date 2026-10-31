#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<errno.h>
#include<time.h>
/*网络初始化*/
int server_netinitializer(const char * ip, int port){
	int server_sockfd;
	struct sockaddr_in addr;
	bzero(&addr,sizeof(addr));
	//网络信息结构体的初始化设置
	addr.sin_family=AF_INET;
	addr.sin_port=htons(port);
	if(!ip)
		addr.sin_addr.s_addr=htonl(INADDR_ANY);
	else
		inet_pton(AF_INET,ip,&addr.sin_addr.s_addr);
	//sock create
	if((server_sockfd = socket(AF_INET,SOCK_STREAM,0))==-1)
		perror("sock create call failed");
	bind(server_sockfd,(struct sockaddr *)&addr,sizeof(addr));//使用自定义的网络信息队sockfd进行绑定
	listen(server_sockfd,128);//设置开启监听，设置链接队列大小
	return server_sockfd;
}

//服务器启动接口
int server_start(int shutdown){
	//单进程非阻塞模型

	//网络初始化
	int server_fd = server_netinitializer(NULL,8080);

	//client_sockfd数组，保存所有连接成功的sockfd，client_sock_array 初始化-1
    #define MAX_CLI 10
	int client_sock_array[MAX_CLI];
	memset(client_sock_array,-1,sizeof(client_sock_array));

	//server_sockfd设置，变为非阻塞sockfd
	int flag = fcntl(server_fd,F_GETFL);
	fcntl(server_fd,F_SETFL,flag | O_NONBLOCK);

	//循环等待连接即可
	while(!shutdown){
	  //调用accept 查看是否有连接请求，如果有，建立连接，而后存储client_sock
	  struct sockaddr_in cli;
	  socklen_t len = sizeof(cli);
	  int cfd = accept(server_fd,(struct sockaddr *)&cli,&len);
	  if(cfd>0){
		  //新连接也设置非阻塞
		  int f = fcntl(cfd,F_GETFL);
		  fcntl(cfd,F_SETFL,f|O_NONBLOCK);
		  //找空位存入数组
		  int i;
		  for(i=0;i<MAX_CLI;i++)
		  {
			  if(client_sock_array[i]==-1){
				  client_sock_array[i]=cfd;
				  break;
			  }
		  }
		  if(i>=MAX_CLI)
		  {
			  close(cfd);
		  }
		  else
		  {
			  char ipbuf[INET_ADDRSTRLEN];
			  inet_ntop(AF_INET,&cli.sin_addr,ipbuf,sizeof(ipbuf));
			  //连接成功先发：hi ip connect successly
			  /*char welcome[256];
			  snprintf(welcome,sizeof(welcome),"hi %s connext successly\n",ipbuf);
			  send(cfd,welcome,strlen(welcome),0);*/
		  }
	  }
	  else{
		  if(errno !=EAGAIN && errno != EWOULDBLOCK){
			  perror("accept");
		  }
	  }
	  //遍历client_sock数组，读取每一个sock，如果有请求数据则处理，没有继续，遍历到末尾
	  char buf[256];
	  for(int i=0;i<MAX_CLI;i++){
		  int sock = client_sock_array[i];
		  if(sock==-1)continue;

		  bzero(buf,sizeof(buf));
		  int recvlen;
		  //recv非阻塞读取
		  recvlen=recv(sock,buf,sizeof(buf)-1,MSG_DONTWAIT);
		  if(recvlen>0){
			  //读取到数据，处理
			  buf[recvlen]='\0';
			  if(strstr(buf,"time")!=NULL){
				  time_t t = time(NULL);
				  char tm[256];
				  ctime_r(&t,tm);
				  send(sock,tm,strlen(tm),0);
			  }
			  else{
				  const char * rep= "please tryagain\n";
				  send(sock,rep,strlen(rep),0);
				  //const char * rep = "hi %s connect successy\n";
				 // char welcome[256];
				 // snprintf(welcome,sizeof(welcome),rep,ipbuf);
				 // send(sock,welcome,strlen(welcome),0);
			  }
		  }
		  else if(recvlen<0){
			  if(errno==EAGAIN){
				  //recv非阻塞返回 无数据
			  }
			  else {
				  perror("recv call failed");
				  close(sock);
				  client_sock_array[i]=-1;
		      }
		  }
		  else{
			  //读取sockfd返回0,客户端断开
			  close(sock);
			  client_sock_array[i]=-1;
		  }
	  }
	  //复位
	}
	close(server_fd);
	for(int i=0;i<MAX_CLI;i++){
		if(client_sock_array[i]!=-1)
			close(client_sock_array[i]);
	}
	return 0;
}
int main(void){
	server_start(0);//启动
	return 0;
}
