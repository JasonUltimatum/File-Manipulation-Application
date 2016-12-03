#include "common.h"
#include <openssl/md5.h>

void timer(int sig)  
{  
        if(SIGALRM == sig)  
        {  
                printf("time up, server off duty\n");  
                //just exit, we don't cleaup des
				exit(1);
        }  
  
        return ;  
}  

char *VCHAR="0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ/_.,+-";

int isFileNameValid(char * str){
	
	int i=0;
	int j=0;
	
	int val = 0;
	for(i=0; i<strlen(str);i++){
		val = 0;
		for(j=0; j<strlen(VCHAR); j++){
			if(VCHAR[j] == str[i]){
				//valid
				val = 1;
				break;
			}
		}
		
		if(val == 0){
			return 0;
		}
	}
	
	return 1;
	
	
	
}

int main(int argc, char *argv[])
{
	
MD5_CTX c;
unsigned char md5buf[4096]; /* buffer to keep data */
unsigned char md5_sum[16]; /* output checksum */	
	
	if(argc < 2)
	{
		fprintf(stderr,"Usage:\n");
		fprintf(stderr,"tserver [-d] [-t seconds] port\n");
		exit(1);
	}
G_IS_LOG = 0;
	signal(SIGALRM, timer);
	
		int port = 6000;
		int isDebug = 0;
		int exitSeconds = 300;
		int pos = 1;
		while(1){
			if(strcmp(argv[pos],"-d")  == 0){
				isDebug = 1;
				G_IS_LOG = 1;
				pos = pos +1;
			}
			else if(strcmp(argv[pos],"-t") == 0){
				exitSeconds = atoi(argv[pos+1]);
				pos = pos +2;
			}
			else{
				break;
			}
		}
	alarm(exitSeconds);
	port = atoi(argv[pos]);
	
	//Input the file name
	MD5_CTX md5;  
	char *buff = (char*)malloc(BUFFSIZE);
	char *cmd = (char*)malloc(BUFFSIZE);

	unsigned char ss[16];
	int offset;
	int length;
	char filename[FILE_NAME_MAX_SIZE];
	bzero(filename,FILE_NAME_MAX_SIZE);

	//Create socket
	int sockfd,connfd;
	struct sockaddr_in svraddr,clientaddr;
	bzero(&svraddr,sizeof(svraddr));
	bzero(&svraddr,sizeof(clientaddr));
	
	svraddr.sin_family=AF_INET;
	svraddr.sin_addr.s_addr=htonl(INADDR_ANY);
	svraddr.sin_port=htons(port);

	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
		perror("socket");
		exit(1);
	}

	//bind	
	if(bind(sockfd,(struct sockaddr*)&svraddr,sizeof(svraddr))<0)
	{
		perror("bind");
		exit(1);
	}

	//listen
	if(listen(sockfd,LISTENQ)<0)
	{
		perror("listen");
		exit(1);
	}

	while(1)
	{
		socklen_t length=sizeof(clientaddr);

		//accept
		connfd=accept(sockfd,(struct sockaddr*)&clientaddr,&length);
		tlog("accept new conn %d %s",1,"");
		if(connfd<0)
		{
			perror("connect");
			exit(1);
		}
	
		//read message
		char msgtype;
		int pos = 0;
		int temp_len = 0;
		char ch;
		int len = recvMessage(connfd, &msgtype, buff);
		buff[len] = '\0';
		
		tlog("\n\n\n\n\n\nget msg ",(char)msgtype,NULL);		
		printf("len: %d\n",len);
		printf("string: %s\n",buff);

		if((msgtype&0xff) == FILETYPE_REQ){
			tlog("get filename: ",0,buff);
			if(!isFileNameValid(buff)){
				char *str = "file not exist or no permission";
				tlog("FILETYPE_ERR:",0,str);
				
				sendMessage(connfd, FILETYPE_ERR, strlen(str), str);		
			}
			else{
				sprintf(cmd,"file %s",buff);
				FILE *fp = popen(cmd, "r");
				char ch;
				pos = 0;
				while ((ch=fgetc(fp))!=EOF)
				{
					if(ch == '\t'){
						ch = ' ';
					}
					cmd[pos++] = ch;
				}	
				pclose(fp);
				
				cmd[pos] = '\0';
				tlog("run 'file' command:",0,cmd);
				sendMessage(connfd, FILETYPE_RSP, strlen(cmd), cmd);			
				tlog("send message: ",0,cmd);
			}
		}
		else if((msgtype&0xff) == CHECKSUM_REQ){
			memcpy(&offset,buff,4);
			offset = ntohl(offset);
			memcpy(&length,buff+4,4);
			length = ntohl(length);
			
			strcpy(filename,buff+8);
			tlog("get offset: ",offset,NULL);
			tlog("get length: ",length,NULL);

			tlog("get filename: ",0,filename);		
			
			FILE *fp = fopen(filename, "r");
			
			if(fp == NULL){
				tlog("ERROR: no such file: ",0,filename);
				sendErrorMsg(connfd, CHECKSUM_ERR,  "no such file");	
				
			}
			else{
				

					tlog("ALL OK, to read file",0,NULL);
						pos = 0;
						temp_len = offset;					
					if(offset > 0){

						while ( temp_len > 0 )
						{
							if(fread(&ch, 1,1, fp) !=0){
								temp_len --;
							}
						}


					}		
					
								
					
						pos = 0;
						temp_len = length;
						int isjudge = 1;
						if(temp_len < 0){
							isjudge = 0;
						}
						
						while (fread(&ch, 1,1, fp) !=0)
						{
							if(isjudge> 0){
								if(temp_len-- <= 0){
									break;
								}
							}	
							
							buff[pos++] = ch;
							
						}	
						
						buff[pos] = '\0';
						tlog("checksum pos is ",pos,"");
						if((isjudge > 0 && pos != length)||pos == 0){
							sendErrorMsg(connfd,CHECKSUM_ERR,"length not right");
						}	
						else{
							
MD5_Init(&c);
MD5_Update(&c, buff,pos );
MD5_Final(md5_sum, &c);

			int i;
			char temp[255];
			char temp2[255];
			

			for( i=0; i<16; i++ ){
				sprintf(temp,"%02x",(char)md5_sum[i]);
				sprintf(temp2+i*2,"%s", temp+strlen(temp)-2);
			}
			temp2[33] = '\0';
			tlog("checksum: ",0,temp2);

							sendMessage(connfd, CHECKSUM_RSP, 32, temp2);
							tlog("send message: ",CHECKSUM_RSP,"");
						}
					
					fclose(fp);	
			}			
			
			
		}
		else if((msgtype&0xff) == DOWNLOAD_REQ){
			memcpy(&offset,buff,4);
			offset = ntohl(offset);
			memcpy(&length,buff+4,4);
			length = ntohl(length);
			
			strcpy(filename,buff+8);
			tlog("get offset: ",offset,NULL);
			tlog("get length: ",length,NULL);

			tlog("get filename: ",0,filename);		
			
			FILE *fp = fopen(filename, "r");
			
			if(fp == NULL){
				tlog("ERROR: no such file: ",0,filename);
				sendErrorMsg(connfd, DOWNLOAD_ERR,  "no such file");	
				
			}
			else{
				

					tlog("ALL OK, to read file",0,NULL);
						pos = 0;
						temp_len = offset;					
					if(offset > 0){

						while ( temp_len > 0 )
						{
							if(fread(&ch, 1,1, fp) !=0){
								temp_len --;
							}
						}


					}		
					
			

						pos = 0;
						temp_len = length;
						int isjudge = 1;
						if((int)length < 0){
							isjudge = 0;
						}
						
						
						while (fread(&ch, 1,1, fp) !=0)
						{
							if(isjudge> 0){
								if(temp_len-- <= 0){
									break;
								}
							}	
							
								buff[pos++] = ch;
							
						}	
						
						buff[pos] = '\0';
						
						tlog("download pos is ",pos,"");
						
						if((isjudge > 0 && pos != length) || pos == 0){
							tlog("not right pos :",pos,"");
							sendErrorMsg(connfd,DOWNLOAD_ERR,"length not right");
						}	
						else{

							sendMessage(connfd, DOWNLOAD_RSP, pos, buff);
							tlog("send message: ",DOWNLOAD_RSP,"");
						}

					fclose(fp);	
			}
			
			
		}
		else{
			sendMessage(connfd, UNKNOWN_FAIL, 12, "erro command");
		}
	}
	return 0;
}
