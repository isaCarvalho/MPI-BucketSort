#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "mpi.h"

#define maxNum 200000 // Maior numero do range
#define tam 50000 // Quantidade de numeros a serem ordenados

int k = 0;

/**
*  E estrutura do balde e formada por:
*  vetor que tem os numeros do balde
*  a ultima posicao que foi inserido o elelmento
*/
typedef struct balde
{
	int *vet;
	int ultimo;
} Balde;

// Funcao para a odenacao dos baldes
int compare(const void * a, const void * b)
{
  if ( *(int*)a <  *(int*)b ) return -1;
  if ( *(int*)a == *(int*)b ) return 0;
  if ( *(int*)a >  *(int*)b ) return 1;
}

// Funcao que ordena cada balde do vetor de baldes, que sera paralelizada
void ordena(int *vet, int n)
{
	qsort(vet, n, sizeof(int), compare);
}

// mapeia a posicao do elemento
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Funcao que insere um elemento ja no balde correto
void inserir(Balde *baldes, int num, int numBaldes)
{
	int pos = map(num, 0, maxNum, 0, numBaldes);
	baldes[pos].vet[baldes[pos].ultimo] = num;
	baldes[pos].ultimo++;
}

void imprimir(int *vet, int n)
{
	for (int i = 0; i < n; i++)
		printf("%d ", vet[i]);

	printf("\n--------------------------------------------------\n");
}

// Funcao que libera a area alocada para os baldes
void liberar(Balde *baldes, int numBaldes)
{
	for (int i =0; i< numBaldes; i++)
		free(baldes[i].vet);

	free(baldes);
}

void main()
{
    MPI_Init(NULL, NULL);

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	int* arr = (int*) malloc(tam * sizeof(int));
	int* vetFinal = (int*) malloc(tam * sizeof(int));
	Balde *baldes = (Balde*) malloc((world_size-1)*sizeof(Balde));

	double start, end;

	if (world_rank == 0) 
	{
		start = MPI_Wtime();

		srand(time(NULL));

		// vetor inicial que contem numeros aleatorios de 0 a maxNum
		for (int i = 0; i < tam; i++)
			arr[i] = (rand() % maxNum) + 1;

		for (int i = 0; i < (world_size-1); i++)
		{
			baldes[i].vet = (int*) malloc(tam*sizeof(int));
			baldes[i].ultimo = 0;
		}

		for (int i = 0; i < tam; i++)
			inserir(baldes, arr[i], world_size-1);

		for (int i = 0; i < (world_size-1); i++)
		{
			// printf("\nO processo 0 enviou o vetor para o processo %d: \n", i+1);
			// imprimir(baldes[i].vet, baldes[i].ultimo);
			MPI_Send(baldes[i].vet, baldes[i].ultimo, MPI_INT, i+1, 0, MPI_COMM_WORLD);
		}

		for (int i = 0; i < (world_size-1); i++)
		{		
			MPI_Recv(baldes[i].vet, tam, MPI_INT, i+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			// printf("\nO processo 0 recebeu o vetor recebeu do processo %d: \n", i+1);
			// imprimir(baldes[i].vet, baldes[i].ultimo);

			for (int j = 0; j < baldes[i].ultimo; j++)
			{
				vetFinal[k] = baldes[i].vet[j];
				k++;
			}
		}

		end = MPI_Wtime();

		printf("\n\nTANRAAAAAAAAAAAAAAAAAAAAAN:\n\n");
		imprimir(vetFinal, tam);

		printf("\nTime MPI: %lf\n", end - start);
	}
	else
	{
		MPI_Status status;

		int i = world_rank-1;
		int tamLimpo;
		int* vetLimpo = (int*) malloc(tam*sizeof(int));

		MPI_Recv(vetLimpo, tam, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		MPI_Get_count(&status, MPI_INT, &tamLimpo);

		// printf("\nO processo %d recebeu o vetor recebeu do processo 0: \n", world_rank);
		// imprimir(vetLimpo, tamLimpo);

		ordena(vetLimpo, tamLimpo);

		// printf("\nO processo %d enviou o vetor para o processo 0: \n", world_rank);
		// imprimir(vetLimpo, tamLimpo);
		MPI_Send(vetLimpo, tamLimpo, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}
