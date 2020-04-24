/* $Id: serial.c,v 1.2 2006/03/28 09:21:05 fred Exp $ */
#include <stdio.h>
#include <stdlib.h>

FILE *fd; 
int TotalEvenWords = 0, TotalOddWords = 0, TotalWords = 0;

const int BUFSIZE = 132;

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

void
CountWords()
{ 
   int bDone = 0;
   char inLine[BUFSIZE];

   while (!bDone) {
      bDone = (GetNextLine(fd, inLine) == EOF);
      if (!bDone) {
         TotalWords += GetWordAndLetterCount(inLine) ;
      }
   }
}

int
main(int argc, char * argv[])
{
   if ((fd = fopen("InFile1.txt", "r")) == NULL) {
      perror("fopen");
      return EXIT_FAILURE;
   }
   
   CountWords();
   
   fclose(fd);
   
   printf("Total Words = %8d\n\n", TotalWords);
   printf("Total Even Words = %7d\nTotal Odd Words  = %7d\n", TotalEvenWords, TotalOddWords);

   return EXIT_SUCCESS;
}
