#ifndef __MYDEBUS_H__
#define __MYDEBUS_H__

#include <dbus/dbus.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define SIGNAL_SEND_NAME "otdr.signal.send"
#define SIGNAL_GET_NAME "otdr.signal.get"
#define METHOD_SEND_NAME "otdr.method.send"
#define METHOD_GET_NAME "otdr.method.get"

void sendsignal(char* sigvalue);
void query(char* param);
void reply_to_method_call(DBusMessage* msg, DBusConnection* conn);
void listen();
void receive(int* socket);

#endif