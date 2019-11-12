#ifndef __MYCLIENT_H__
#define __MYCLIENT_H__

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>

#define VERSION "0.0.1"
#define STAGE "alpha"
#define LAST_EDIT_TIME "2019.10.31"
#define IP "192.168.22.130"
#define PORT 8888
#define MAX_SIZE 2048

void *connection_handler(void *threadid);
void *thread_get(void *socket);
void version(); //返回网络模块版本号
//void set_server(); //设置服务器的ip地址，端口号
//void connect_server() //连接到服务器

#endif