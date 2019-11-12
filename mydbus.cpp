#include "mydbus.h"

/**
 * Connect to the DBUS bus and send a broadcast signal
 */
void sendsignal(unsigned char *sigvalue,int len)
{
   DBusMessage *msg;
   DBusMessageIter args;
   DBusConnection *conn;
   DBusError err;
   int ret;
   dbus_uint32_t serial = 0;

   char *tmp = (char*)calloc(2*len+1,sizeof(char));   

   byte2chars(sigvalue,len,tmp);

   // initialise the error value
   dbus_error_init(&err);

   // connect to the DBUS system bus, and check for errors
   conn = dbus_bus_get_private(DBUS_BUS_SESSION, &err);
   if (dbus_error_is_set(&err))
   {
      fprintf(stderr, "Connection Error (%s)\n", err.message);
      dbus_error_free(&err);
   }
   if (NULL == conn)
   {
      exit(1);
   }

   // register our name on the bus, and check for errors
   ret = dbus_bus_request_name(conn, SEND_SIGNAL_DBUS_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
   if (dbus_error_is_set(&err))
   {
      fprintf(stderr, "Name Error (%s)\n", err.message);
      dbus_error_free(&err);
   }
   if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret)
   {
      exit(1);
   }

   // create a signal & check for errors
   msg = dbus_message_new_signal(SEND_SIGNAL_PATH_NAME,      // object name of the signal
                                 SEND_SIGNAL_INTERFACE_NAME, // interface name of the signal
                                 SEND_SIGNAL_NAME);          // name of the signal
   if (NULL == msg)
   {
      fprintf(stderr, "Message Null\n");
      exit(1);
   }

   // append arguments onto signal
   dbus_message_iter_init_append(msg, &args);

   if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &tmp))
   {
      free(tmp);
      fprintf(stderr, "Out Of Memory!\n");
      exit(1);
   }

   // send the message and flush the connection
   if (!dbus_connection_send(conn, msg, &serial))
   {
      fprintf(stderr, "Out Of Memory!\n");
      exit(1);
   }
   dbus_connection_flush(conn);

   printf("Signal Sent!\n");

   // free the message and close the connection
   dbus_message_unref(msg);
   free(tmp);
   dbus_connection_close(conn);
}

/**
 * Listens for signals on the bus
 */
void receive(int *sock_desc)
{
   DBusMessage *msg;
   DBusMessageIter args;
   DBusConnection *conn;
   DBusError err;
   int ret;
   char *sigvalue;

   printf("Listening for signals\n");

   // initialise the errors
   dbus_error_init(&err);

   // connect to the bus and check for errors
   conn = dbus_bus_get_private(DBUS_BUS_SESSION, &err);
   if (dbus_error_is_set(&err))
   {
      fprintf(stderr, "Connection Error (%s)\n", err.message);
      dbus_error_free(&err);
   }
   if (NULL == conn)
   {
      exit(1);
   }

   // request our name on the bus and check for errors
   ret = dbus_bus_request_name(conn, GET_SIGNAL_DBUS_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
   if (dbus_error_is_set(&err))
   {
      fprintf(stderr, "Name Error (%s)\n", err.message);
      dbus_error_free(&err);
   }
   if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret)
   {
      exit(1);
   }

   // add a rule for which messages we want to see
   char string[50];
   sprintf(string, "%s%s%s", "type='signal',interface='",GET_SIGNAL_INTERFACE_NAME, "'");
   dbus_bus_add_match(conn, string, &err); // see signals from the given interface
   dbus_connection_flush(conn);
   if (dbus_error_is_set(&err))
   {
      fprintf(stderr, "Match Error (%s)\n", err.message);
      exit(1);
   }

   // loop listening for signals being emmitted
   while (true)
   {

      // non blocking read of the next available message
  
      dbus_connection_read_write(conn, 0);
      msg = dbus_connection_pop_message(conn);

      // loop again if we haven't read a message
      if (NULL == msg)
      {
         //sleep(1);
         continue;
      }

      // check if the message is a signal from the correct interface and with the correct name
      if (dbus_message_is_signal(msg, GET_SIGNAL_INTERFACE_NAME, GET_SIGNAL_NAME))
      {

         // read the parameters
         if (!dbus_message_iter_init(msg, &args))
            fprintf(stderr, "Message Has No Parameters\n");
         else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
            fprintf(stderr, "Argument is not string!\n");
         else
            dbus_message_iter_get_basic(&args, &sigvalue);

         int slength = strlen(sigvalue);
         unsigned char *re = (unsigned char*)calloc(slength/2+1,sizeof(unsigned char));

         chars2byte(sigvalue,re);

         //printf("Got Signal as ASCII: %s,%d\n", re, slength/2);
         // if (slength/2 > 0)
         // {
         //    printf("Got Signal as HEX: ");
         //    for (int i = 0; i < slength/2; ++i)
         //       printf("0X%02x,", (unsigned int)(*(re+i)));
         //    printf("\n");
         // }

         send(*sock_desc, re, slength/2, 0);
         free(re);

         printf("Signal Got!\n");

         //struct timeval tv;
         //gettimeofday(&tv,NULL);
         //printf("<-millisecond get:%ld\n",tv.tv_sec*1000 + tv.tv_usec/1000); 

      }

      // free the message
      dbus_message_unref(msg);
   }
   // close the connection
   dbus_connection_close(conn);
}

/**
 * Call a method on a remote object
 */
void query(char *param)
{
   DBusMessage *msg;
   DBusMessageIter args;
   DBusConnection *conn;
   DBusError err;
   DBusPendingCall *pending;
   int ret;
   bool stat;
   dbus_uint32_t level;

   printf("Calling remote method with %s\n", param);

   // initialiset the errors
   dbus_error_init(&err);

   // connect to the system bus and check for errors
   conn = dbus_bus_get_private(DBUS_BUS_SESSION, &err);
   if (dbus_error_is_set(&err))
   {
      fprintf(stderr, "Connection Error (%s)\n", err.message);
      dbus_error_free(&err);
   }
   if (NULL == conn)
   {
      exit(1);
   }

   // request our name on the bus
   ret = dbus_bus_request_name(conn, "test.method.caller", DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
   if (dbus_error_is_set(&err))
   {
      fprintf(stderr, "Name Error (%s)\n", err.message);
      dbus_error_free(&err);
   }
   if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret)
   {
      exit(1);
   }

   // create a new method call and check for errors
   msg = dbus_message_new_method_call("test.method.server",  // target for the method call
                                      "/test/method/Object", // object to call on
                                      "test.method.Type",    // interface to call on
                                      "Method");             // method name
   if (NULL == msg)
   {
      fprintf(stderr, "Message Null\n");
      exit(1);
   }

   // append arguments
   dbus_message_iter_init_append(msg, &args);
   if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &param))
   {
      fprintf(stderr, "Out Of Memory!\n");
      exit(1);
   }

   // send message and get a handle for a reply
   if (!dbus_connection_send_with_reply(conn, msg, &pending, -1))
   { // -1 is default timeout
      fprintf(stderr, "Out Of Memory!\n");
      exit(1);
   }
   if (NULL == pending)
   {
      fprintf(stderr, "Pending Call Null\n");
      exit(1);
   }
   dbus_connection_flush(conn);

   printf("Request Sent\n");

   // free message
   dbus_message_unref(msg);

   // block until we recieve a reply
   dbus_pending_call_block(pending);

   // get the reply message
   msg = dbus_pending_call_steal_reply(pending);
   if (NULL == msg)
   {
      fprintf(stderr, "Reply Null\n");
      exit(1);
   }
   // free the pending message handle
   dbus_pending_call_unref(pending);

   // read the parameters
   if (!dbus_message_iter_init(msg, &args))
      fprintf(stderr, "Message has no arguments!\n");
   else if (DBUS_TYPE_BOOLEAN != dbus_message_iter_get_arg_type(&args))
      fprintf(stderr, "Argument is not boolean!\n");
   else
      dbus_message_iter_get_basic(&args, &stat);

   if (!dbus_message_iter_next(&args))
      fprintf(stderr, "Message has too few arguments!\n");
   else if (DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&args))
      fprintf(stderr, "Argument is not int!\n");
   else
      dbus_message_iter_get_basic(&args, &level);

   printf("Got Reply: %d, %d\n", stat, level);

   // free reply and close connection
   dbus_message_unref(msg);
   dbus_connection_close(conn);
}

void reply_to_method_call(DBusMessage *msg, DBusConnection *conn)
{
   DBusMessage *reply;
   DBusMessageIter args;
   bool stat = true;
   dbus_uint32_t level = 21614;
   dbus_uint32_t serial = 0;
   char *param = "";

   // read the arguments
   if (!dbus_message_iter_init(msg, &args))
      fprintf(stderr, "Message has no arguments!\n");
   else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args))
      fprintf(stderr, "Argument is not string!\n");
   else
      dbus_message_iter_get_basic(&args, &param);

   printf("Method called with %s\n", param);

   // create a reply from the message
   reply = dbus_message_new_method_return(msg);

   // add the arguments to the reply
   dbus_message_iter_init_append(reply, &args);
   if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_BOOLEAN, &stat))
   {
      fprintf(stderr, "Out Of Memory!\n");
      exit(1);
   }
   if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &level))
   {
      fprintf(stderr, "Out Of Memory!\n");
      exit(1);
   }

   // send the reply && flush the connection
   if (!dbus_connection_send(conn, reply, &serial))
   {
      fprintf(stderr, "Out Of Memory!\n");
      exit(1);
   }
   dbus_connection_flush(conn);

   // free the reply
   dbus_message_unref(reply);
}

/**
 * Server that exposes a method call and waits for it to be called
 */
void listen()
{
   DBusMessage *msg;
   DBusMessage *reply;
   DBusMessageIter args;
   DBusConnection *conn;
   DBusError err;
   int ret;
   char *param;

   printf("Listening for method calls\n");

   // initialise the error
   dbus_error_init(&err);

   // connect to the bus and check for errors
   conn = dbus_bus_get_private(DBUS_BUS_SESSION, &err);
   if (dbus_error_is_set(&err))
   {
      fprintf(stderr, "Connection Error (%s)\n", err.message);
      dbus_error_free(&err);
   }
   if (NULL == conn)
   {
      fprintf(stderr, "Connection Null\n");
      exit(1);
   }

   // request our name on the bus and check for errors
   ret = dbus_bus_request_name(conn, "test.method.server", DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
   if (dbus_error_is_set(&err))
   {
      fprintf(stderr, "Name Error (%s)\n", err.message);
      dbus_error_free(&err);
   }
   if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret)
   {
      fprintf(stderr, "Not Primary Owner (%d)\n", ret);
      exit(1);
   }

   // loop, testing for new messages
   while (true)
   {
      // non blocking read of the next available message
      dbus_connection_read_write(conn, 0);
      msg = dbus_connection_pop_message(conn);

      // loop again if we haven't got a message
      if (NULL == msg)
      {
         sleep(1);
         continue;
      }

      // check this is a method call for the right interface & method
      if (dbus_message_is_method_call(msg, "test.method.Type", "Method"))
         reply_to_method_call(msg, conn);

      // free the message
      dbus_message_unref(msg);
   }

   // close the connection
   dbus_connection_close(conn);
}

void byte2chars(unsigned char *rbuf,int len,char *re)
{
   
   for (int i = 0; i < len; i++)
   {
      int byte = (unsigned int)(*(rbuf + i));
      //printf("0X%02x,", byte);
      if (byte == 0x00)
      {
         *(re + i * 2) = 0x01;
         *(re + i * 2 + 1) = 0x01;
      }
      else if ((byte > 0x00) && (byte <= 0x7f))
      {
         *(re + i * 2) = 0x03;
         *(re + i * 2 + 1) = byte - 0x00;
      }
      else if (byte == 0x80)
      {
         *(re + i * 2) = 0x02;
         *(re + i * 2 + 1) = 0x02;
      }
      else if ((byte > 0x80) && (byte <= 0xff))
      {
         *(re + i * 2) = 0x04;
         *(re + i * 2 + 1) = byte - 0x80;
      }
      //printf("0X%02x,0X%02x,", tmp[i * 2], tmp[i * 2 + 1]);
   }
}

void chars2byte(const char *rbuf, unsigned char *re)
{
   int length = strlen(rbuf) / 2;
   for (int i = 0; i < length; i++)
   {
      if ((*(rbuf + 2 * i)) == 0x01)
         *(re + i) = 0x00;
      else if ((*(rbuf + 2 * i)) == 0x02)
         *(re + i) = 0x80;
      else if ((*(rbuf + 2 * i)) == 0x03)
         *(re + i) = (*(rbuf + 2 * i + 1));
      else if ((*(rbuf + 2 * i)) == 0x04)
         *(re + i) = (*(rbuf + 2 * i + 1)) + 0x80;
   }
}

// int main(int argc, char** argv)
// {
//    if (2 > argc) {
//       printf ("Syntax: mydbus [send|receive|listen|query] [<param>]\n
//                send: signal send.\n
//                receive: signal receive.
//                listen: method listen.
//                query: method query.");
//       return 1;
//    }
//    setenv("DISPLAY",":0",true);
//    char* param = "no param";
//    if (3 >= argc && NULL != argv[2]) param = argv[2];
//    if (0 == strcmp(argv[1], "send"))    //signal
//       sendsignal(param);
//    else if (0 == strcmp(argv[1], "receive")) //signal
//       receive();
//    else if (0 == strcmp(argv[1], "listen"))  //function
//       listen();
//    else if (0 == strcmp(argv[1], "query"))   //function
//       query(param);
//    else {
//       printf ("Syntax: mydbus [send|receive|listen|query] [<param>]\n
//                send: signal send.\n
//                receive: signal receive.
//                listen: method listen.
//                query: method query.");
//       return 1;
//    }
//    return 0;
// }