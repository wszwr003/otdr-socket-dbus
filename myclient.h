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
#include "mydbus.h"

#define VERSION "0.0.1"
#define STAGE "alpha"
#define LAST_EDIT_TIME "2019.10.31"
#define IP "192.168.22.130"
#define PORT 8888
#define MAX_SIZE 100

void *connection_handler(void *threadid);
void *thread_get(void *socket);

#endif