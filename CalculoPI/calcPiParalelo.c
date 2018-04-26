#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

static long numPassos = 1000000;
double passo;
int numThreads;
// guarda cada pedaço da soma do valor de Pi
double *somaParcial ;

void *calcula(void *my_thread);

int main(int argc, char *argv[])
{
  struct timeval mytime;
  int segs1, segs2;
  int msegs1, msegs2;
  double tempoTotal = 0.0, tempoInicial = 0.0, tempoFinal = 0.0;

  switch(argc)
  {
     case 0:
     case 1:
        fprintf(stderr, "\nUso:\n\t %s <quantidade de threads a utilizar para calcular o PI ( >= 2) >", argv[0]);
        exit(EXIT_FAILURE);
        break;

     case 2:
        numThreads = atoi(argv[1]);
        break;
  }

  somaParcial = malloc(numThreads * sizeof(double));

  long i;
  double pi = 0.0;

  passo = 1.0/(double) numPassos;

  // threads
  pthread_t thrds[numThreads];

  // tempo inicial
  gettimeofday(&mytime, NULL);
  segs1  = (int)mytime.tv_sec;
  msegs1 = (int)mytime.tv_usec;

  // criando as threads
  for(i = 0; i < numThreads; i++)
  {
     pthread_create(&thrds[i], NULL, calcula, (void *) i);
     printf("Thread i = %ld\n", i);
  }

  // join para todas as threads
  for(i = 0; i < numThreads; i++) pthread_join(thrds[i], NULL);

  // soma os resultados intermediarios
  for(i = 0; i < numThreads; i++) pi += somaParcial[i];

  // tempo final
  gettimeofday(&mytime, NULL);
  segs2  = (int)mytime.tv_sec;
  msegs2 = (int)mytime.tv_usec;

  tempoInicial = (double)segs1 + ((double)msegs1/1000000.0);
  tempoFinal = (double)segs2 + ((double)msegs2/1000000.0);
  tempoTotal = tempoFinal - tempoInicial;
  printf("tempo = %12.6f\n",tempoTotal);
  printf("Pi: %.30f\n", pi);
}

void *calcula(void *my_thread){
  long i, min, max;
  long lmy_thread = (long)my_thread;

  double x, soma = 0.0;

  min = (numPassos / numThreads) * lmy_thread;
  if (lmy_thread == numThreads - 1)
    max = numPassos - 1;
  else
    max = (numPassos / numThreads) * (lmy_thread + 1) - 1;

  for (i=min; i <= max; i++)
  {
    x = (i+0.5)*passo;
    soma = soma + 4.0/(1.0+x*x);

  }
    printf("soma = %f\n", soma);

  somaParcial[lmy_thread] = soma*passo;
}
