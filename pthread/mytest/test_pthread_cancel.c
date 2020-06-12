#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define NUMTHREADS 4


void *helloFunc(void *pArg) 
{ 
    while(1)
    {
        printf("Hello Thread\n"); 
        sleep(1);
    }
}

int main() 
{	
    pthread_t hThread;

    pthread_create(&hThread, NULL, helloFunc, NULL);

    //pthread_join(hThread, NULL);
    sleep(5);
    pthread_cancel(hThread);

    return 0; 
}


