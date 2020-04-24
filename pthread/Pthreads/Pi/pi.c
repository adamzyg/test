#include <stdio.h>

#define NUM_STEPS 1000000

double step, pi;

void main()
{  int i;	
   double x, sum = 0.0;

   step = 1.0/(double) NUM_STEPS;
   for (i=0; i< NUM_STEPS; i++){
      x = (i+0.5)*step;
      sum = sum + 4.0/(1.0 + x*x);
   }

   pi = step * sum;
   printf("Pi = %12.9f\n",pi);
}
