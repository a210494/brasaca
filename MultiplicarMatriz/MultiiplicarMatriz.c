/*
Disciplina  :  Processamento de Alto Desempenho
Autor       :  Adalberto José Brasaca - R.A. 210494
Finalidade  :  Calcular a multiplicação de duas matrizes
Data        :  20/04/2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define BILHAO  1000000000L;
// matriz bidimensional temporária
int** MatrizTemp = NULL;
// estruturas que guardam as dimensoõss das duas matrizes
struct dimensaoMatriz dimensaoMatrizA, dimensaoMatrizB;
// valor de retorno da função de consistência
int okConsistencia = 0;
// variáveis para armazenar tempo inicial e tempo final
clock_t timeIni, timeFim;

// armazena o tempo do cálculo da multiplicação das matrizes
double tempo;

/*
Função                :  carregarMatriz
Parâmetros de Entrada :  nomeArquivoMatriz - vetor de char contendo o nome do arquivo texto com a matriz a ser carregada
Finalidade            :  ler um arquivo texto e carregar os valores em uma matriz multimensional alocada dinamicamente
Retorno               :  ponteiro para a matriz de inteiros carregada
*/

int** carregarMatriz(char* nomeArquivoMatriz, int linhas, int colunas);

/*
Estrutura   :  dimensaoMatriz
Finalidade  :  responsável por armazenar os valores de quantidade de linhas e colunas da matriz do arquivo texto lido
*/

struct dimensaoMatriz
{
    int linhas;
    int colunas;
};

/*
Função               :  calcularDimensaoMatriz
Parâmetro de Entrada :  nomeArquivoMatriz - vetor de char contendo o nome do arquivo texto com a matriz a ser carregada
Finalidade           :  ler um arquivo texto com uma matriz e calcular a quantidade de linhas e colunas
Retorno              :  estrutura dimensaoMatriz contendo a quantidade de linhas e colunas
*/

struct dimensaoMatriz calcularDimensaoMatriz(char* nomeArquivoMatriz);

/*
Função                :  fazerConsistenciaMatrizes
Parâmetros de Entrada :  dois inteiros, contendo a quantidade de colunas da primeira matriz e a quantidade de linhas da segunda matriz
Finalidade            :  verificar se pode ser feita a multiplicação das matrizes, analisando se o número de colunas da primeira é igual ao número de linhas da segunda
Retorno               :  um inteiro ( 0 = falso e 1 = verdadeiro )
*/
int fazerConsistenciaMatrizes(int qtdeColunasMatrizA, int qtdeLinhasMatrizB);

/*
Função                :  multiplicarMatrizes
Parâmetros de Entrada :  ponteiro para MatrizA, linhas da MatrizA, colunas da MatrizA, ponteiro para a MatrizB, linhas da MatrizB, colunas da MatrizB
Finalidade            :  realizar a multiplicação de duas matrizes
Retorno               :  uma matriz com o resultado da multiplicação
*/

int** multiplicarMatrizes(int** matrizA, int linhasA, int colunasA, int** matrizB, int linhasB, int colunasB);

/*
Função                :  salvarMatriz
Parâmetros de Entrada :  ponteiro para MatrizC (resultado da multiplicação), linhas da MatrizC, colunas da MatrizC, nome do arquivo texto a salcar
Finalidade            :  salvar a matriz resultante da multiplicação em um arquivo texto
Retorno               :  void
*/

void salvarMatriz(int** mat, unsigned int lines, unsigned int columns, char *fileName);

int main(int argc, char *argv[])
{
    // Nomes dos arquivos contendo as matrizes
    char *nomeArquivoMatrizA, *nomeArquivoMatrizB, *nomeArquivoMatrizC;
    // ponteiros para as matrizes de inteiros carregadas
    int **MatrizA = NULL, **MatrizB = NULL, **MatrizC = NULL;

    switch(argc)
    {
     case 0:
     case 1:
     case 2:
         fprintf(stderr, "\nUso:\n\t %s <nome do arquivo da primeira matriz> <nome do arquivo da segunda matriz>  =====> resultado da multiplicacao serial na tela.\n ou", argv[0]);
         fprintf(stderr, "\n\t %s <nome do arquivo da primeira matriz> <nome do arquivo da segunda matriz> <nome do arquivo de saida> =====> resultado da multiplicacao serial no arquivo. ou\n", argv[0]);
         fprintf(stderr, "\n\t %s <nome do arquivo da primeira matriz> <nome do arquivo da segunda matriz> <nome do arquivo de saida> <quantidade de threads ( >= 2 )>\n", argv[0]);

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
         break;
     }

     printf("\nCalculando as dimensoes das matrizes... ");

    // Calculando dimensões das matrizes e guardando em estruturas
    dimensaoMatrizA = calcularDimensaoMatriz(nomeArquivoMatrizA);
    dimensaoMatrizB = calcularDimensaoMatriz(nomeArquivoMatrizB);

    printf("OK !\n");

    printf("Realizando a consistencia das matrizes... ");

    /*
     Verificando se é permitida a operação de multiplicação entre as matrizes
    (a quantidade de colunas da MatrizA deve ser igual à quantidade de linhas da MatrizB)
    OBS.: No primeiro argumento está sendo passada 1 coluna a menos em virtude da maneira como está sendo calculada
    a quantidade de colunas das matrizes (utilizando 2 espaços em branco como token). Como há 2 espaços após o último
    número de cada linha, no cálculo da dimensão o resultado da quantidade de colunas é erroneamente acrescido de 1.
    Portanto, para passar a quantidade correta, é subtraída uma unidade da quantidade de colunas.
    */
    okConsistencia = fazerConsistenciaMatrizes((dimensaoMatrizA.colunas - 1), dimensaoMatrizB.linhas);

    if(!okConsistencia)
    {
        printf("\n\nErro ! Nao e possivel multiplicar as matrizes.\nNumero de colunas da primeira matriz e diferente do numero de linhas da segunda.\n");
        getchar();
        exit(EXIT_FAILURE);
    }

    printf("OK !\n");

    printf("Carregando as matrizes a partir dos arquivos textos... ");

    // Carregando os dados dos arquivos texto para as matrizes dinâmicas multidimensionais
    MatrizA = carregarMatriz(nomeArquivoMatrizA, dimensaoMatrizA.linhas, dimensaoMatrizA.colunas);
    MatrizB = carregarMatriz(nomeArquivoMatrizB, dimensaoMatrizB.linhas, dimensaoMatrizB.colunas);

    printf("OK !\n");

    MatrizC = multiplicarMatrizes(MatrizA, dimensaoMatrizA.linhas, dimensaoMatrizA.colunas, MatrizB, dimensaoMatrizB.linhas, dimensaoMatrizB.colunas);

    printf("Salvando a matriz resultante em disco... ");

    salvarMatriz(MatrizC, dimensaoMatrizA.linhas, dimensaoMatrizB.colunas - 1, nomeArquivoMatrizC);

    printf("OK !\n\n");

    printf("Tempo de processamento da multiplicacao das matrizes = %.10f segundos\n\n", tempo);

    return 0;
}

int** carregarMatriz(char* nomeArquivoMatriz, int linhas, int colunas)
{
    FILE *arquivoMatriz;
    char linhaTemp[10000], *token;
    int lin, col;

    // Abrindo o arquivo de matriz no modo leitura
    arquivoMatriz = fopen(nomeArquivoMatriz, "r");

    // Verificando se não conseguiu ler o arquivo
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

    // Verificando se não conseguiu ler o arquivo
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
};

int fazerConsistenciaMatrizes(int qtdeColunasMatrizA, int qtdeLinhasMatrizB)
{
    int okConsistencia = 0;

    if(qtdeColunasMatrizA == qtdeLinhasMatrizB)
    {
        okConsistencia = 1;
    }
    return okConsistencia;
}

int** multiplicarMatrizes(int** matrizA, int linhasA, int colunasA, int** matrizB, int linhasB, int colunasB)
{
    MatrizTemp = (int **) calloc(linhasA, sizeof(int *));
    for(int col = 0; col < (colunasA - 1); col++)
    {
        MatrizTemp[col] = (int *) calloc((colunasB - 1), sizeof(int));
    }

    timeIni = clock();

    for(int i = 0; i < linhasA; i++)
        for(int j = 0; j < colunasB; j++)
            for(int k = 0; k < colunasA; k++)
            {
                MatrizTemp[i][j] += matrizA[i][k] * matrizB[k][j];
            }

    timeFim = clock();

    tempo = ( (double) (timeFim - timeIni) ) / CLOCKS_PER_SEC;

    return MatrizTemp;
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

