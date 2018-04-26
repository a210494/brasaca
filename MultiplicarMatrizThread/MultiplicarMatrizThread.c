/*
Disciplina  :  Processamento de Alto Desempenho
Autor       :  Adalberto Jos� Brasaca - R.A. 210494
Finalidade  :  Calcular a multiplica��o de duas matrizes - PARALELA
Data        :  20/04/2018
*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// ponteiros para as matrizes de inteiros carregadas
int **MatrizA = NULL, **MatrizB = NULL, **MatrizC = NULL;
// matriz bidimensional tempor�ria
int **MatrizTemp = NULL;
// estruturas que guardam as dimenso�ss das duas matrizes
struct dimensaoMatriz dimensaoMatrizA, dimensaoMatrizB;
// valor de retorno da fun��o de consist�ncia
int okConsistencia = 0;
// N�mero de threads
int numThreads;
// variveis para c�lculo do tempo de execu��o
clock_t timeIni, timeFim;

// c�lculo do tempo
double tempo;

/*
Estrutura   :  dimensaoMatriz
Finalidade  :  respons�vel por armazenar os valores de quantidade de linhas e colunas da matriz do arquivo texto lido
*/

struct dimensaoMatriz
{
    int linhas;
    int colunas;
};

/*
Fun��o                :  carregarMatriz
Par�metros de Entrada :  nomeArquivoMatriz - vetor de char contendo o nome do arquivo texto com a matriz a ser carregada
Finalidade            :  ler um arquivo texto e carregar os valores em uma matriz multimensional alocada dinamicamente
Retorno               :  ponteiro para a matriz de inteiros carregada
*/

int** carregarMatriz(char* nomeArquivoMatriz, int linhas, int colunas);

/*
Fun��o               :  calcularDimensaoMatriz
Par�metro de Entrada :  nomeArquivoMatriz - vetor de char contendo o nome do arquivo texto com a matriz a ser carregada
Finalidade           :  ler um arquivo texto com uma matriz e calcular a quantidade de linhas e colunas
Retorno              :  estrutura dimensaoMatriz contendo a quantidade de linhas e colunas
*/

struct dimensaoMatriz calcularDimensaoMatriz(char* nomeArquivoMatriz);

/*
Fun��o                :  fazerConsistenciaMatrizes
Par�metros de Entrada :  dois inteiros, contendo a quantidade de colunas da primeira matriz e a quantidade de linhas da segunda matriz
Finalidade            :  verificar se pode ser feita a multiplica��o das matrizes, analisando se o n�mero de colunas da primeira � igual ao n�mero de linhas da segunda
Retorno               :  um inteiro ( 0 = falso e 1 = verdadeiro )
*/
int fazerConsistenciaMatrizes(int qtdeColunasMatrizA, int qtdeLinhasMatrizB);

/*
Fun��o                :  salvarMatriz
Par�metros de Entrada :  ponteiro para MatrizC (resultado da multiplica��o), linhas da MatrizC, colunas da MatrizC, nome do arquivo texto a salcar
Finalidade            :  salvar a matriz resultante da multiplica��o em um arquivo texto
Retorno               :  void
*/

void salvarMatriz(int** mat, unsigned int lines, unsigned int columns, char *fileName);

// Fun��o a ser utilizada pelas threads
void* multiplicar(void* fatia);

int main(int argc, char* argv[])
{
  // Nomes dos arquivos contendo as matrizes
  char *nomeArquivoMatrizA, *nomeArquivoMatrizB, *nomeArquivoMatrizC;
  // ponteiro para o grupo de threads
  pthread_t* thread;
  long i;

 switch(argc)
    {
     case 0:
     case 1:
     case 2:
         fprintf(stderr, "\nUso:\n\t %s <nome do arquivo da primeira matriz> <nome do arquivo da segunda matriz>  =====> resultado da multiplicacao serial na tela.\n ou", argv[0]);
         fprintf(stderr, "\n\t %s <nome do arquivo da primeira matriz> <nome do arquivo da segunda matriz> <nome do arquivo de saida> <n�mero de threads ( >= 2 ) > =====> resultado da multiplicacao serial no arquivo. ou\n", argv[0]);

         exit(EXIT_FAILURE);
         break;
     case 3:
         nomeArquivoMatrizA = argv[1];
         nomeArquivoMatrizB = argv[2];
         nomeArquivoMatrizC = NULL;

         break;
     case 4:
         nomeArquivoMatrizA = argv[1];
         nomeArquivoMatrizB = argv[2];
         nomeArquivoMatrizC = argv[3];

         break;
     case 5:
         nomeArquivoMatrizA = argv[1];
         nomeArquivoMatrizB = argv[2];
         nomeArquivoMatrizC = argv[3];
         numThreads = atoi(argv[4]);
         break;
    }

  // Calculando dimens�es das matrizes e guardando em estruturas
  dimensaoMatrizA = calcularDimensaoMatriz(nomeArquivoMatrizA);
  dimensaoMatrizB = calcularDimensaoMatriz(nomeArquivoMatrizB);

  /*
    Verificando se � permitida a opera��o de multiplica��o entre as matrizes
   (a quantidade de colunas da MatrizA deve ser igual � quantidade de linhas da MatrizB)
   OBS.: No primeiro argumento est� sendo passada 1 coluna a menos em virtude da maneira como est� sendo calculada
   a quantidade de colunas das matrizes (utilizando 2 espa�os em branco como token). Como h� 2 espa�os ap�s o �ltimo
   n�mero de cada linha, no c�lculo da dimens�o o resultado da quantidade de colunas � erroneamente acrescido de 1.
   Portanto, para passar a quantidade correta, � subtra�da uma unidade da quantidade de colunas.
  */
  okConsistencia = fazerConsistenciaMatrizes((dimensaoMatrizA.colunas - 1), dimensaoMatrizB.linhas);

  if(!okConsistencia)
  {
      printf("\nErro ! Nao e possivel multiplicar as matrizes.\nNumero de colunas da primeira matriz e diferente do numero de linhas da segunda.\n");
      getchar();
//      free(MatrizTemp);
      exit(EXIT_FAILURE);
  }

 // Carregando os dados dos arquivos texto para as matrizes din�micas multidimensionais
  MatrizA = carregarMatriz(nomeArquivoMatrizA, dimensaoMatrizA.linhas, dimensaoMatrizA.colunas);
  MatrizB = carregarMatriz(nomeArquivoMatrizB, dimensaoMatrizB.linhas, dimensaoMatrizB.colunas);

  thread = (pthread_t*) malloc(numThreads*sizeof(pthread_t));

  // marcar inicio do tempo de execu��o em paralelo
  timeIni = clock();

  // N�o ir� utilizar threads no c�lculo se o n�mero passado como argumento for igual a 1.
  for (long i = 1; i < numThreads; i++)
  {
    // criando cada thread que ir� trabalhar na sua propria faixa de dados
    if (pthread_create (&thread[i], NULL, multiplicar, (void*)i) != 0 )
    {
      perror("N�o consegui criar a thread");
      free(thread);
      exit(-1);
    }
  }

  // Se passar o n�mero 1 o c�lculo ser� feito pela thread principal = SERIAL
  multiplicar(0);

  // thread principal aguardando as outras completarem o c�lculo
  for (i = 1; i < numThreads; i++)
      pthread_join(thread[i], NULL);

  // marcando o tempo de final da execu��o em paralelo
  timeFim = clock();

  // calculando o tempo
  tempo = ( (double) (timeFim - timeIni) ) / CLOCKS_PER_SEC;

  printf("Tempo de execucao em paralelo = %.10f segundos\n\n", tempo);

  free(thread);

  salvarMatriz(MatrizC, dimensaoMatrizA.linhas, dimensaoMatrizB.colunas - 1, nomeArquivoMatrizC);

  return 0;
}

void* multiplicar(void* fatia)
{
  // recuperar o n�mero de fatias
  long s = (long)fatia;
  long inicio = (s * dimensaoMatrizA.linhas)/numThreads;
  long fim = ((s+1) * dimensaoMatrizA.linhas)/numThreads;
  int i,j,k;

  MatrizC = (int **) calloc(dimensaoMatrizA.linhas, sizeof(int *));
    for(int col = 0; col < (dimensaoMatrizA.colunas - 1); col++)
    {
        MatrizC[col] = (int *) calloc((dimensaoMatrizB.colunas - 1), sizeof(int));
    }

  printf("Calculando a fatia %ld (da linha %ld ate %ld)\n", s, inicio, fim-1);

  for (i = inicio; i < fim; i++)
  {
    for (j = 0; j < dimensaoMatrizA.linhas; j++)
    {
       MatrizC[i][j] = 0;
       for ( k = 0; k < (dimensaoMatrizB.colunas - 1); k++)
           MatrizC[i][j] += MatrizA[i][k]*MatrizB[k][j];
    }
  }
  printf("Calculo da fatia %ld terminada.\n\n", s);

  return 0;
}

int** carregarMatriz(char* nomeArquivoMatriz, int linhas, int colunas)
{
    FILE *arquivoMatriz;
    char linhaTemp[10000], *token;
    int lin, col;

    // Abrindo o arquivo de matriz no modo leitura
    arquivoMatriz = fopen(nomeArquivoMatriz, "r");

    // Verificando se n�o conseguiu ler o arquivo
    if(arquivoMatriz == NULL)
    {
        printf("\nErro ! Nao foi possivel abrir o arquivo %s contendo a matriz...\n", nomeArquivoMatriz);
        getchar();
        fclose(arquivoMatriz);
        exit(EXIT_FAILURE);
    }

    MatrizTemp = (int **) calloc(linhas, sizeof(int *));
    for(int col = 0; col < colunas; col++)
    {
        MatrizTemp[col] = (int *) calloc(colunas, sizeof(int));
    }

    lin = 0;

    while(fgets(linhaTemp, sizeof(linhaTemp), arquivoMatriz) != NULL){
       token = strtok(linhaTemp, "  ");
       col = 0;

       while(token != NULL)
       {
           MatrizTemp[lin][col] = atoi(token);
           token = strtok(NULL, "  ");
           col++;
       }
       lin++;
    }

    return MatrizTemp;

    fclose(arquivoMatriz);
}

struct dimensaoMatriz calcularDimensaoMatriz(char *nomeArquivoMatriz)
{
    FILE *arquivoDimensao;
    struct dimensaoMatriz tamanhoMatriz;
    char linhaTemp[10000], *token = NULL;
    int linhas, colunas;

     // Abrindo o arquivo de matriz no modo leitura
    arquivoDimensao = fopen(nomeArquivoMatriz, "r");

    // Verificando se n�o conseguiu ler o arquivo
    if(arquivoDimensao == NULL)
    {
        printf("\nErro ! Nao foi possivel abrir o arquivo %s contendo a matriz...\n", nomeArquivoMatriz);
        getchar();
        fclose(arquivoDimensao);
        exit(EXIT_FAILURE);
    }

    linhas = 0;

    while(fgets(linhaTemp, sizeof(linhaTemp), arquivoDimensao) != NULL){
       token = strtok(linhaTemp, "  ");
       colunas = 0;

       while(token != NULL)
       {
           token = strtok(NULL, "  ");
           colunas++;
       }
       linhas++;

    }

    tamanhoMatriz.linhas = linhas;
    tamanhoMatriz.colunas = colunas;

    fclose(arquivoDimensao);

    return(tamanhoMatriz);
}

int fazerConsistenciaMatrizes(int qtdeColunasMatrizA, int qtdeLinhasMatrizB)
{
    int okConsistencia = 0;

    if(qtdeColunasMatrizA == qtdeLinhasMatrizB)
    {
        okConsistencia = 1;
    }
    return okConsistencia;
}

void salvarMatriz(int** mat, unsigned int lines, unsigned int columns, char *fileName)
{
  register unsigned int i, j;
  register long int value;
  FILE *arq;

  if (fileName == NULL)
        arq = stdout;
  else
  {
      arq = fopen(fileName, "w");
      if (arq == NULL)
      {
        perror("Nao consegui abrir o arquivo de saida.\n");
        exit(EXIT_FAILURE);
      }
  }

  for (i=0; i<lines; i++)
  {
      for(j=0; j<columns; j++)
      {
          value = mat[i][j];
          if (value >= 0)
            fprintf(arq, " %ld  ",value);
          else
            fprintf(arq, "%ld  ", value);
      }
      fprintf(arq, "\n");
  }

  if (fileName != NULL)
     fclose(arq);
  return;
}


