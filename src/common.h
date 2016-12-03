
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h> 
#include <netdb.h>

#define PORT 6000
#define LISTENQ 20
#define BUFFSIZE 10000000
#define FILE_NAME_MAX_SIZE 512

#define FILETYPE_REQ 0xea
#define FILETYPE_RSP 0xe9
#define FILETYPE_ERR 0xe8
#define CHECKSUM_REQ 0xca
#define CHECKSUM_RSP 0xc9
#define CHECKSUM_ERR 0xc8
#define DOWNLOAD_REQ 0xaa
#define DOWNLOAD_RSP 0xa9
#define DOWNLOAD_ERR 0xa8
#define UNKNOWN_FAIL 0x51

extern int G_IS_LOG;

int sendMessage(int sockfd, char msgtype, ssize_t len, char *buffer);
int sendErrorMsg(int sockfd, char msgtype, char * str);

int recvMessage(int socketfd, char *msgtype, char *buffer);
int findChr(char *str, char c);
int findChrRev(char *str, char c);

int isFileExist(char *filename);

int tlog(const char *formatStr,int i, const char *msg);


#endif /* !COMMON_H */
