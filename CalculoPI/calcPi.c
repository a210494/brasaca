#include <stdio.h>
#include <time.h>

static long num_steps = 1000000;
double step;
clock_t timeIni, timeFim;
double tempo;

int main ()
{
  int i;
  double x, pi, sum = 0.0;

  timeIni = clock();

  step = 1.0/(double) num_steps;
  for (i=0; i<num_steps; i++)
  {
     x = (i+0.5)*step;
     sum = sum + 4.0/(1.0+x*x);
  }
  pi = step * sum;

  timeFim = clock();

  tempo = ( (double) (timeFim - timeIni) ) / CLOCKS_PER_SEC;

  printf("Valor de pi: %.30f\n", pi);
  printf("O tempo de execucao foi: %.15f\n", tempo);


  return 0;
}







