// Metodo de ordenacao BucketSort - versao sequencial


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define maxNum 200000 // Maior numero do range
#define numBaldes 1 // Numero de baldes
#define tam 50000 // Quantidade de numeros a serem ordenados

/* E estrutura do balde e formada por:
*  vetor que tem os numeros do balde
*  a ultima posicao que foi inserido o elelmento
*/
typedef struct balde
{
	int *vet;
	int ultimo = 0;
} Balde;

// Funcao para a odenacao dos baldes
int compare(const void * a, const void * b)
{
  if ( *(int*)a <  *(int*)b ) return -1;
  if ( *(int*)a == *(int*)b ) return 0;
  if ( *(int*)a >  *(int*)b ) return 1;
}

// Funcao que ordena cada balde do vetor de baldes, que sera paralelizada
void ordenaBalde(Balde *baldes)
{
	for (int i = 0; i < numBaldes; i++)
		qsort(baldes[i].vet, tam, sizeof(int), compare);
}

// // Funcao que insere um elemento ja no balde correto
// void inserir(Balde *baldes, int num)
// {
// 	if (num <= 10)
// 	{
// 		baldes[0].vet[baldes[0].ultimo] = num;
// 		baldes[0].ultimo++;
// 	}
// 	else
// 	{
// 		int pos = ceil(num / numBaldes);
// 		baldes[pos].vet[baldes[pos].ultimo] = num;
// 		baldes[pos].ultimo++;
// 	}
// }

// mapeia a posicao do elemento
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Funcao que insere um elemento ja no balde correto
void inserir(Balde *baldes, int num)
{
	int pos = map(num, 0, maxNum, 0, numBaldes);
	baldes[pos].vet[baldes[pos].ultimo] = num;
	baldes[pos].ultimo++;
}

// Funcao que imprime todos os elementos de cada balde
void imprimir(Balde *baldes)
{
	for (int i = 0; i < numBaldes; i++)
	{
		printf("\n%d Balde: ", i+1);
		for (int j = 0; j < tam; j++)
			if (baldes[i].vet[j] != 0)
			printf("%d ", baldes[i].vet[j]);

		printf("\n--------------------------------------------------\n");
	}
}

// Funcao que libera a area alocada para os baldes
void liberar(Balde *baldes)
{
	for (int i =0; i< numBaldes; i++)
		free(baldes[i].vet);

	free(baldes);
}

int main(void)
{
	srand(time(NULL));

	// vetor inicial que contem numeros aleatorios de 0 a maxNum
	int* arr = (int*) malloc(tam * sizeof(int));

	//printf("Arr: \n");
	for (int i = 0; i < tam; i++)
	{
		arr[i] = (rand() % maxNum) + 1;
		//printf("%d ", arr[i]);
	}
	//printf("\n\n");

	// alocacao dos baldes
	Balde *baldes = (Balde*) malloc(numBaldes * sizeof(Balde));

	for(int i = 0; i < numBaldes; i++)
	{
		baldes[i].vet = (int*) malloc(tam * sizeof(int));
	}

	// insercao de cada elemento do array arr no balde correto
	for (int i = 0; i < tam; i++)
		inserir(baldes, arr[i]);

	// ordenacao dos baldes
	ordenaBalde(baldes);
	//imprimir(baldes);

	// insercao dos elementos ordenados no vetor final
	int *vetFinal = (int*) malloc(tam * sizeof(int));

	int k = 0;
	for (int i = 0; i < numBaldes; i++)
	{
		for (int j = 0; j < tam; j++)
		{
			if (baldes[i].vet[j] != 0)
			{
				vetFinal[k] = baldes[i].vet[j];
				k++;
			}
		}
	}

	printf("Vet Final:\n");
	for (int i = 0; i < tam; i++)
		printf("%d ", vetFinal[i]);
	printf("\n\n");

	// liberacao das areas alocadas
	free(arr);
	free(vetFinal);
	liberar(baldes);

	return 0;
}
