#include <stdio.h>
#define _USE_BSD 1
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_SEND_BUF 1000
#define MAX_RECV_BUF 1000
#define MAX_DATA 1000
static int nchildren;
static pid_t *pids;

extern int errno;

int errexit(const char *format,...);

int connectUDP(const char *service,int portnum);
int connectsock(const char *service,int portnum,const char *transport);
void handler(int);

/*------------------------------------------------------------------------------------
 * connectsock-Allocate and connect socket for UDP
 *------------------------------------------------------------------------------------
*/

int connectsock(const char *service,int portnum,const char *transport)
{
/*
Arguments:
*service   - service associated with desired port
*transport - name of the transport protocol to use
*/
struct sockaddr_in server;                                                //an internet endpoint address

int server_socket,type,b,l,accept_socket,num;                             //two socket descriptors for listening and accepting 

memset(&server,0,sizeof(server));

server.sin_addr.s_addr=htons(INADDR_ANY);                                 //INADDR_ANY to match any IP address
server.sin_family=AF_INET;                                                //family name
server.sin_port=htons(portnum);                                              //port number

 
/*
 * to determine the type of socket
 */

if(strcmp(transport,"udp")==0)
{
type=SOCK_DGRAM;
}
else
{
type=SOCK_STREAM;
}


server_socket=socket(AF_INET,type,0);                                    //allocate a socket

if(server_socket<0)
{
printf("Socket can't be created\n");
exit(0);
}

/* to set the socket options- to reuse the given port multiple times */
num=1;

if(setsockopt(server_socket,SOL_SOCKET,SO_REUSEPORT,(const char*)&num,sizeof(num))<0)
{
printf("setsockopt(SO_REUSEPORT) failed\n");
exit(0);
}


/* bind the socket to known port */
b=bind(server_socket,(struct sockaddr*)&server,sizeof(server));

if(b<0)
{
printf("Error in binding\n");
exit(0);
}

return server_socket;

}

/*------------------------------------------------------------------------
 * connectUDP-connect to a specified UDP service on specified host
 -------------------------------------------------------------------------*/
int connectUDP(const char *service,int portnum)
{
/*
 Arguments:
 *service-service associated with desired port
 */
 return connectsock(service,portnum,"udp");
}

void handler(int sig)
{
	int i;
	//void pr_cpu_time(void);
	/* terminate all children */
	for (i = 0; i < nchildren; i++)
	kill(pids[i], SIGTERM);
	while (wait(NULL) > 0); /* wait for all children */
	exit(0);		
}

void child_main(int i, int listenfd, int addrlen)
{
	int connfd;
	int filefunction(int);
	socklen_t clilen;
	struct sockaddr *cliaddr;
	cliaddr = malloc(addrlen);
	printf("\nchild %ld created\n", (long) getpid());

	clilen = addrlen;		
	filefunction(listenfd); /* process the request */
	close(listenfd);

}

int errexit(const char* format,...)
{
	va_list args;
	va_start(args,format);
	vfprintf(stderr,format,args);
	va_end(args);
	exit(1);
}


pid_t child_make(int i, int listenfd, int addrlen)
{
	pid_t pid;
	void child_main(int, int, int);
	if ( (pid = fork()) > 0)
	{
		
		return (pid); /* parent */
	}
        else if((pid==0))
	{
        
	//printf("child created:%d",getpid());
	child_main(i, listenfd, addrlen); /* never returns */
	}
}

/*
 main - connectionless preforked server
 */

int main(char argc,char *argv[])
{
	nchildren = atoi(argv[1]);
        int portnum=atoi(argv[2]);

	socklen_t addrlen;
	void sig_int(int);
	pid_t child_make(int, int, int);

	char *service="echo";    
        int alen;
        int msock;

        msock=connectUDP(service,portnum);
        printf("Listening to client\n");
        int i;

	pids = calloc(nchildren, sizeof(pid_t));
	for (i = 0; i < nchildren; i++)
		pids[i] = child_make(i, msock, addrlen); /* parent returns */

	(void) signal(SIGCHLD,handler);
    
        while(1)
        {
            pause();
	}
}


/** function to handle the client connection*/
int filefunction(int ssock)
{
         char msg[1000];
         char send_buf[MAX_SEND_BUF];

     while(1)
      {
	 struct sockaddr_in sin;
	 int alen = sizeof(sin);
         int data_len;
        
         data_len = recvfrom(ssock,msg,MAX_DATA,0, (struct sockaddr *)&sin, &alen);
                        
         if(data_len)
         {      
              printf("connected to preforked connectionless server\n");          
              printf("Filename recieved: %s\n", msg);       
         }
                        
            
         int file;      //for reading local file(server file)
         if((file = open(msg,O_RDONLY))<0)
         {                       
               printf("File not found\n");  
                printf("Client disconnected\n");              
         }
         else
         {                    
               printf("File opened successfully\n");
                     
               ssize_t read_bytes,sent_bytes;                 
               char send_buf[MAX_SEND_BUF];                 
    
               if( (read_bytes = read(file, send_buf, MAX_RECV_BUF)) > 0 )
               {
                     printf("%s",send_buf);
                     if( (sent_bytes = sendto(ssock, send_buf, read_bytes, 0, (struct sockaddr *)&sin,sizeof(sin))) < read_bytes )
                     {
                     	printf("send error");
                     	return -1;
                     } 
	  	     printf("\nchild %ld terminated\n", (long) getpid());
                     printf("\nclient disconnected\n");  
		     close(file);  
		                         
               }  
                      		       	         
         }

   }
      	 return 0;
}

