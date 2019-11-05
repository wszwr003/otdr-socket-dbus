#ifndef __TEST_MYDEBUS_H__
#define __TEST_MYDEBUS_H__

#include <dbus/dbus.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/socket.h>

#define OTDR_SEND_SIGNAL_DBUS_NAME "get.signal.otdr"
#define GET_SIGNAL_DBUS_NAME "get.signal.middle"
#define GET_SIGNAL_PATH_NAME "/get/signal/middle"
#define GET_SIGNAL_INTERFACE_NAME "get.signal.middle"
#define GET_SIGNAL_NAME "get"

#define OTDR_GET_SIGNAL_DBUS_NAME "send.signal.otdr"
#define SEND_SIGNAL_DBUS_NAME "send.signal.middle"
#define SEND_SIGNAL_PATH_NAME "/send/signal/middle"
#define SEND_SIGNAL_INTERFACE_NAME "send.signal.middle"
#define SEND_SIGNAL_NAME "send"

#define METHOD_SEND_NAME "otdr.method.send"
#define METHOD_GET_NAME "otdr.method.get"

#endif