#include "myclient.h"
#include "mydbus.h"
#include "ipv4.h"

void *connection_handler(void *threadid)
{
    long threadnum = (long)threadid;
    int * sock_desc;
    struct sockaddr_in serv_addr;
    char sbuff[MAX_SIZE],rbuff[MAX_SIZE];
    pthread_t thread_b;
    sock_desc = (int*)malloc(sizeof(int));
    if((* sock_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Failed creating socket\n");
        //return -1;
    }     

    bzero((char *) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(IP);
    serv_addr.sin_port = htons(PORT);

    if ((connect(* sock_desc, (struct sockaddr *) &serv_addr, sizeof (serv_addr))) < 0) {
        printf("Failed to connect to server!\n");
        //return -1;
    }

    printf("Connected successfully client\n");
    if( pthread_create( &thread_b , NULL ,  thread_get , sock_desc))
    {
        perror("Could not create thread_get");
        //return -1;
    }
    
    sleep(1);

    receive(sock_desc);
    // while(1) //send
    // {
    //     //printf("======For thread send: %ld ======\n", threadnum);
    //     //printf("please input send msg:");
    //     fgets(sbuff, MAX_SIZE , stdin);
    //     send(*sock_desc,sbuff,strlen(sbuff),0);
    //     printf(">>>>>> sended : %s ",sbuff);
    //     //printf("======Thread send end : %ld ======\n\n", threadnum);
    //     sleep(1);
    // }
    close(*sock_desc);
    return 0;
}

void *thread_get(void *socket)  //get
{
    char rbuff[MAX_SIZE];
    int sock_desc = *((int *)socket);
    while(1) //get
    {
        //printf("======For thread get ======\n");
        if(recv(sock_desc,rbuff,MAX_SIZE,0)==0)
            printf("Error");
        else
        {
            printf("1.Got socket: ");
            fputs(rbuff,stdout);
            printf("\n");
            sendsignal(rbuff);
        }   
        bzero(rbuff,MAX_SIZE);
        //printf("======Thread get end ======\n\n");
        sleep(1);
    }
}

void version(){
   sendsignal(VERSION);
}

int main()
{
    setenv("DISPLAY",":0",true);
    pthread_t thread_a;
    if( pthread_create( &thread_a , NULL ,  connection_handler , (void*) 1) < 0)
    {
        perror("Could not create thread connection_handler!");
        return 1;
    }
    sleep(1);
    pthread_exit(NULL);
    return 0;
}