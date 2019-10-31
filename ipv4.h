#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>	

using namespace std;

void set_localhost(const char * ip,const char * gw,const char * netmask);
//void get_localhost();