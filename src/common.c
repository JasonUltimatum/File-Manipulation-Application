#include "common.h"
int G_IS_LOG = 1;
int tlog(const char *preMsg,int i, const char *msg){
	//printf("G_IS_LOG %d\n",G_IS_LOG);
	if(G_IS_LOG){
		if(msg == NULL){
			printf("%s\t0x%x \n",preMsg,i);
		}
		else{
			printf("%s\t0x%x\t%s \n",preMsg,i,msg);
		}
	}
}

int isFileExist(char *filename){
FILE * fp;
 
fp=fopen(filename,"r");
 
if(fp==NULL)
{
return 0;
}
return 1;
}

int findChr(char *str, char c){
	int i = 0;
	int len = strlen(str);
	for(i=0;i<len;i++){
		if(str[i] == c){
			return i;
		}
	}
	return -1;

}


int findChrRev(char *str, char c){
	int i = 0;
	int len = strlen(str);
	for(i=len-1;i>=0;i--){
		if(str[i] == c){
			return i;
		}
	}
	return -1;

}

ssize_t socket_send(int sockfd, const char* buffer, size_t buflen)
{
ssize_t tmp;
size_t total = buflen;
const char *p = buffer;

tlog("socket_send, buflen: %d\n",buflen,NULL);

while(1)
{
    tmp = send(sockfd, p, total, 0);
	tlog("SEND bytes: ",tmp,"");
    if(tmp < 0)
    {
      // 当send收到信号时,可以继续写,但这里返回-1.
      if(errno == EINTR)
{	
 	tlog("EINTR %d %s",1,"");	
        return -1;
}
      // 当socket是非阻塞时,如返回此错误,表示写缓冲队列已满,
      // 在这里做延时后再重试.
      if(errno == EAGAIN)
      {
        usleep(1000);
	tlog("EAGIN, continue %d %s",1,"");
        continue;
      }
	tlog("RETURN -1 %d %s",1,"");
      return -1;
    }

    if((size_t)tmp == total)
{
      return buflen;
}
    total -= tmp;
    p += tmp;
}

return tmp;
}


ssize_t socket_recv(int sockfd, char* buffer,ssize_t len )
{
//read first byte
//read length
//read buffer
char * buf = buffer;
int buflen;

while(1==1)
{
buflen = recv(sockfd, buf,len, 0);
tlog("read buffer %d %s",buflen, "");
if(buflen < 0){

      // 当send收到信号时,可以继续写,但这里返回-1.
      if(errno == EINTR)
        return -1;

      // 当socket是非阻塞时,如返回此错误,表示写缓冲队列已满,
      // 在这里做延时后再重试.
      if(errno == EAGAIN)
      {
        usleep(1000);
        continue;
      }
}
else if(buflen == 0){
//网络中断了
return -1;
}
else if(buflen >  0)
{
    // 由于是非阻塞的模式,所以当errno为EAGAIN时,表示当前缓冲区已无数据可读
    // 在这里就当作是该次事件已处理处.
	buf = buf+buflen;
        len = len-buflen;

	if(len == 0){
		return 0;	
	}

}
}
}

int sendMessage(int socketfd, char msgtype, ssize_t len, char *buffer)
{
	tlog("sendMessage,msgtype:",msgtype,NULL);
	tlog("sendMessage,len:",len,NULL);
	socket_send(socketfd,&msgtype,1);
	int v = htonl(len);
	socket_send(socketfd,(char*)&v,4);
	socket_send(socketfd,buffer,len);
	return 0;

}

int sendErrorMsg(int sockfd, char msgtype, char * str){
	sendMessage(sockfd, msgtype, strlen(str),str);
}

int recvMessage(int socketfd, char *msgtype, char *buffer)
{
	socket_recv(socketfd,msgtype,1);	
	tlog("read message type %x %s", *msgtype,"");
	int len;
	socket_recv(socketfd,(char*)&len,4);	
	len = ntohl(len);
	tlog("read length %d %s",len,"");
    
	if(socket_recv(socketfd,buffer,len) == 0){	
		tlog("read buffer done %d %s",0,"");
	}
	else{
		tlog("ERROR: read buffer not done",0,NULL);
	}
	return len;

}

