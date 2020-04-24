/* $Id: semaphore_threaded_solution.c,v 1.3 2006/03/28 09:20:17 fred Exp $ */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

FILE *fd;
int TotalEvenWords = 0, TotalOddWords = 0, TotalWords = 0;

const int NUMTHREADS = 4;
const int BUFSIZE = 132;

sem_t IoSem;
sem_t VarSem;

int
GetNextLine(FILE *f, char *Line)
{    
   if ((fgets(Line, BUFSIZE, f) == NULL) && feof(f)) {
      return EOF;
   }
   
   return 1;
}

int
GetWordAndLetterCount(char *Line)
{
   int Word_Count = 0, Letter_Count = 0;
   
   for (int i=0; i<BUFSIZE; i++) {
      if ((Line[i]!=' ') && (Line[i]!=0) && (Line[i]!='\n')) {
         Letter_Count++; 
      } else {
         if (Letter_Count % 2) {
            TotalOddWords++; 
            Word_Count++;
            Letter_Count = 0;
         } else {
            TotalEvenWords++;
            Word_Count++;
            Letter_Count = 0;
         }
         if (Line[i]==0) {
            break;
         }
      }
   }
   return Word_Count;     // encode two return values
}

void*
CountWords(void *arg) 
{ 
   int bDone = 0;
   char inLine[BUFSIZE];
   
   while (!bDone) {
      sem_wait(&IoSem);
      bDone = (GetNextLine(fd, inLine) == EOF);
      if (sem_post(&IoSem)) {
         perror("sem_post");
         pthread_exit(NULL);
      }
      
      if (!bDone) {
         sem_wait(&VarSem);
         TotalWords += GetWordAndLetterCount(inLine) ;
         if (sem_post(&VarSem)) {
            perror("sem_post");
            pthread_exit(NULL);
         }
      }
   }
   return NULL;
}

int
main(int argc, char * argv[])
{
   pthread_t hThread[NUMTHREADS];
   int i;

   if (sem_init(&VarSem, 0, 1) || sem_init(&IoSem, 0, 1)) {
      perror("sem_init");
      return EXIT_FAILURE;
   }
   
   if ((fd = fopen("InFile1.txt", "r")) == NULL) {
      perror("fopen");
      return EXIT_FAILURE;
   }
   
   for (i = 0; i < NUMTHREADS; i++) {
      if (pthread_create(&hThread[i], NULL, &CountWords, NULL)) {
         perror("pthread_create");
         return EXIT_FAILURE;
      }
   }
   
   for (i = 0; i < NUMTHREADS; i++) {
      if (pthread_join(hThread[i], NULL)) {
         perror("pthread_join");
         return EXIT_FAILURE;
      }
   }
   
   fclose(fd);
   
   printf("Total Words = %8d\n\n", TotalWords);
   printf("Total Even Words = %7d\nTotal Odd Words  = %7d\n", TotalEvenWords, TotalOddWords);

   return EXIT_SUCCESS;
}
