/**************************************************/
/* Multithreaded C program using the Pthread API  */
/* From Figure 5.5, Operating Systems Concepts,   */
/* Sixth Edition, Silberschatz, Galvin, and Gagne */
/*                                                */
/* To compile me in Linux type:                   */
/*    gcc -o chap5 chap5.c -lpthread              */
/**************************************************/

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>

#include <string.h>

#define closesocket      close
#define PROTOPORT        5555        /* default protocol port number */

extern int               errno;
char   localhost[] = "localhost";    /* default host name            */
#define MAX_THREADS 100

void *socket_test(void *param);

int main(int argc, char *argv[])
{
	pthread_t tid[MAX_THREADS]; /* the thread identifier */
	pthread_attr_t attr; /* set of thread attributes */
	pid_t pid_main;
	pthread_t tid_main;
	unsigned int n_ptrd;
	if (argc != 2) {
		fprintf(stderr, "usage:  a.out <integer value>\n");
		return(1);
	}
	pid_main = getpid();
	tid_main = pthread_self(); 

	printf("PID[%d]   TID[%d]\n", pid_main, tid_main);

	n_ptrd = atoi(argv[1]);
	if (n_ptrd < 0) {
		fprintf(stderr,"%d must be > 0\n", n_ptrd);
		return(1);
	}

	/* get the default attributes */
	pthread_attr_init(&attr);

	for(int i=0; i<n_ptrd; i++)
	{
		/* create the thread */
		pthread_create(&tid[i], &attr, socket_test, &i);
	}

	for(int j=0; j<n_ptrd; j++)
	{
		/* now wait for the thread to exit */
		pthread_join(tid[j],NULL);
	}

	return 0;
}


/* The thread will connect local server(127.0.0.1:5555)
 * send string string=date+tid
 * recv string and echo it
 * */
void *socket_test(void *param)
{
	struct  hostent  *ptrh;   /* pointer to a host table entry			 */
	struct  protoent *ptrp;   /* point to a protocol table entry		 */
	struct  sockaddr_in sad;  /* structure to hold server's address		 */
	int     sd;               /* socket descriptor						 */
	int     port;             /* protocol port number					 */
	char    *host;            /* pointer to host name					 */
	int     send_n;           /* number of characters send				 */
	int     recv_n;           /* number of characters recv				 */

	char    send_buf[100];    /* buffer for send data from the server    */
	char    recv_buf[100];    /* buffer for recv data from the server    */
	unsigned int l_send_buf;

	struct timeval t_start;	  /* start send time(second*1000000+usec)    */
	struct timeval t_end;	  /* end recv time(second*1000000+usec)		 */
	
	pthread_t tid;			  /* this pthread ID						 */

	tid = pthread_self();
	printf("Pthread[%d] begin\n", tid);

	memset((char *)&sad,0,sizeof(sad));  /* clear sockaddr structure */
	sad.sin_family = AF_INET;            /* set family to Internet   */

	/* Check command-line argument for protocol port and extract     */
	/* port number if on is specified.  Otherwise, use the default   */
	/* port value biven by constant PROTOPORT                        */

	port = PROTOPORT;
	sad.sin_port = htons((u_short)port);

	host = localhost;
	ptrh = gethostbyname(host);

	memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);

	if ( ((int)(ptrp = getprotobyname("tcp"))) == 0)
	{ 
		fprintf( stderr, "cannot map \"tcp\" to protocol number\n");
		pthread_exit(1);
	}

	sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
	if (sd < 0)
	{
		fprintf( stderr, "socket creation failed\n");
		pthread_exit(1);
	}

	if (connect(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0)
	{ 
		fprintf( stderr, "connect failed\n");
		pthread_exit(1);
	}

	/* send buffer */
	bzero(send_buf, sizeof(send_buf));
	bzero(recv_buf, sizeof(recv_buf));
	gettimeofday(&t_start, NULL);
	sprintf(send_buf, "%d:%d:%ld", tid, sd, t_start.tv_sec*1000000 + t_start.tv_usec);
	l_send_buf = strlen(send_buf) + 1;  	/* send string with '\0' */
	send_n = send(sd, send_buf, l_send_buf, 0);
	if(send_n > 0)
	{
		fprintf(stderr, "Pthread[%d]: send data length(%d)  original buffer(%s) length(%d)\n",
				send_n, send_buf, l_send_buf);
	}

	/* recv buffer */
	recv_n = recv(sd, recv_buf, sizeof(recv_buf), 0);
	if(recv_n > 0)
	{ 
		fprintf( stderr, "Pthread[%d]: recv data %s\n", tid, recv_buf);
	}
	gettimeofday(&t_end, NULL);
	fprintf(stderr, "Pthread[%d]: spend time(%ld)\n", tid, 
			( (t_end.tv_sec*1000000 + t_end.tv_usec)
			  - (t_start.tv_sec*1000000 + t_start.tv_usec) ) );

	closesocket(sd);

	printf("Pthread[%d] end\n", tid);
	pthread_exit(0);
}
