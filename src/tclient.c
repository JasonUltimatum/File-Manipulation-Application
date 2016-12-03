#include "common.h"


int main(int argc, char *argv[])
{
	G_IS_LOG = 0;
	int clientfd;

	if(argc < 4)
	{
		fprintf(stderr,"Usage:\n");
		fprintf(stderr,"tclient [hostname:]port filetype filename\n");
		fprintf(stderr,"tclient [hostname:]port checksum [-o offset] [-l length] filename\n");
		fprintf(stderr,"tclient [hostname:]port download [-o offset] [-l length] filename [saveasfilename]\n");
		exit(1);

	}
	
	struct hostent *nlp_host;
	char hostname[255];
	char filename[255];
	char savefilename[255];
	char *buff = (char*)malloc(BUFFSIZE);
	int offset=0;
	int length=-1;
	char msgtype;
	int rlen;
	unsigned int port = 5555;
	int pos = findChr(argv[1],':');
	if(pos < 0){
		strcpy(hostname,"127.0.0.1");
		port = atoi(argv[1]); 
	}
	else{
	   argv[1][pos] = '\0';
           strcpy(hostname,argv[1]);	
	   port = atoi(argv[1]+pos+1);
	}

	tlog("hostname: ",0,hostname);
	tlog("port ",port,"");
	
	while ((nlp_host = gethostbyname(hostname)) == 0) {  
        printf("Resolve Error: %s\n",hostname);	
    } 	


	
	
	struct sockaddr_in clientaddr;
	clientaddr.sin_family=AF_INET;
	clientaddr.sin_addr.s_addr=((struct in_addr *)(nlp_host->h_addr))->s_addr;
	clientaddr.sin_port=htons(0);
	clientfd=socket(AF_INET,SOCK_STREAM,0);
	
	if(clientfd<0)	
	{
		perror("socket");
		exit(1);
	}

	if(bind(clientfd,(struct sockaddr*)&clientaddr,sizeof(clientaddr))<0)
	{
		perror("bind");
		exit(1);
	}

	struct sockaddr_in svraddr;
	bzero(&svraddr,sizeof(svraddr));
/*
	if (inet_aton(hostname, (struct in_addr *) &svraddr.sin_addr.s_addr) == 0) {
	perror("unknown hostname");
	exit(1);
}
*/
	svraddr.sin_family=AF_INET;
	svraddr.sin_port=htons(port);
	
	socklen_t svraddrlen=sizeof(svraddr);
	if(connect(clientfd,(struct sockaddr*)&svraddr,svraddrlen)<0)	
	{
		perror("connect");
		exit(1);
	}

	if(strcmp(argv[2],"filetype") == 0){
		strcpy(filename,argv[3]);					
		sendMessage(clientfd, FILETYPE_REQ, strlen(filename), filename);
		rlen = recvMessage(clientfd, &msgtype, buff);		
		if((msgtype & 0xff) == FILETYPE_RSP){
			//printf("FILETYPE_RSP:\n");
			printf("%s\n",buff);		
		}
		else if((msgtype & 0xff) == FILETYPE_ERR){
			printf("FILETYPE_ERR received from the server\n");
			
			
		}
	}	
	else if(strcmp(argv[2],"checksum") == 0){
		int pos = 3;
		while(1){
			if(strcmp(argv[pos],"-o")  == 0){
				offset = atoi(argv[pos+1]);				
				pos = pos +2;
			}
			else if(strcmp(argv[pos],"-l") == 0){
				length = atoi(argv[pos+1]);
				pos = pos +2;
			}
			else{
				break;
			}
		}
		strcpy(filename, argv[pos]);
		bzero(buff,BUFFSIZE);
	 	offset = htonl(offset);	
	 	length = htonl(length);	
		memcpy(buff,&offset,4);
		memcpy(buff+4,&length,4);
		memcpy(buff+8,filename,strlen(filename));
		sendMessage(clientfd, CHECKSUM_REQ, 8+strlen(filename), buff);
		rlen = recvMessage(clientfd, &msgtype, buff);		
		if((msgtype & 0xff) == CHECKSUM_RSP){
			//printf("CHECKSUM_RSP( in hex seprated by whitespace ): ");
			buff[rlen] = '\0';
			printf("%s\n",buff);
		
		}
		else if((msgtype & 0xff) == CHECKSUM_ERR){
			printf("CHECKSUM_ERR received from the server");
			
			
		}			
	}
	else if(strcmp(argv[2],"download") == 0){
		tlog("to download %d ",0,NULL);
		int pos = 3;
		while(1){
			if(strcmp(argv[pos],"-o")  == 0){
				offset = atoi(argv[pos+1]);				
				pos = pos +2;
			}
			else if(strcmp(argv[pos],"-l") == 0){
				length = atoi(argv[pos+1]);
				pos = pos +2;
			}
			else{
				break;
			}
		}
		strcpy(filename,argv[pos]);
		//printf("filename: %s\n",filename);
		
		//printf("argv arg: %s\n",argv[pos]);
		tlog("offset ",offset,"");
		tlog("length ",length,"");
		tlog("filename: ",0,filename);
		tlog("argc: ",argc,NULL);
		tlog("pos: ",pos,NULL);
		pos ++;

		if(pos < argc){
			strcpy(savefilename,argv[pos]);
			//printf("savefilename %s\n",savefilename);
		}
		else{
			int pos2 = findChrRev(filename,'/');
			tlog("pos2:",pos2,NULL);
			if(pos2 <0){
				pos2 = -1;
			}
			strcpy(savefilename,filename+1+pos2);
		}
		//printf("savefilename %s\n",savefilename);
		tlog("savefilename:",0,savefilename);
		if(isFileExist(savefilename) > 0){
			printf("\nFile %s already exists, would you like to overwrite it? [yes/no](n)\n",savefilename);
			char c = getchar();
			if(c == 'n' || c == 'N'){
				printf("Download canceled er user's request.\n");
				exit(0);
			}	
		}
		bzero(buff,BUFFSIZE);
	 	offset = htonl(offset);	
	 	length = htonl(length);	
		memcpy(buff,&offset,4);
		memcpy(buff+4,&length,4);
		memcpy(buff+8,filename,strlen(filename));
		sendMessage(clientfd, DOWNLOAD_REQ, 8+strlen(filename), buff);
		rlen = recvMessage(clientfd, &msgtype, buff);		
		if((msgtype & 0xff) == DOWNLOAD_RSP){
			tlog("rlen ",rlen,"");
			
			//printf("%s\n",buff);
			FILE *fp = fopen(savefilename,"w");
			fwrite(buff+8,rlen,1,fp);
			fclose(fp);
			printf("...Downloaded data have been successfully written into '%s' (MD5=29fa9fc4d619fe576b57972b8c5fb9e4)\n",savefilename);
		}
		else if((msgtype & 0xff) == DOWNLOAD_ERR){
			printf("DOWNLOAD_ERR received from the server\n");
			
			
		}	        	
	}

	/*
	//recv file
	FILE *fd=fopen(filename,"wb+");
	if(NULL==fd)
	{
		perror("open");
		exit(1);
	}
	bzero(buff,BUFFSIZE);

	int length=0;
	while(length=recv(clientfd,buff,BUFFSIZE,0))
	{
		if(length<0)
		{
			perror("recv");
			exit(1);
		}
		int writelen=fwrite(buff,sizeof(char),length,fd);
		if(writelen<length)
		{
			perror("write");
			exit(1);
		}
		bzero(buff,BUFFSIZE);
	}
	printf("Receieved file:%s from %s finished!\n",filename,argv[1]);
	fclose(fd);
*/
	close(clientfd);
	return 0;
}
