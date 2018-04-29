/* 
 * test_ordenamento.c: recebe um inteiro, cria uma array de valores aleatorios com este inteiro e ordena através do bubble sort. O objetivo é testar inits, yelds e joins durante a execução de funções demoradas e rápidas. A cada 100 ordenamentos a função libera intencionalmente seu recurso.
 */

#include	"../include/support.h"
#include	"../include/cthread.h"
#include	<stdio.h>
#include	<stdlib.h>
#include 	<time.h>
#include 	"../src/cthread.c"

#define		MAX_SIZE	250
#define		MAX_THR		10

int vetor[MAX_SIZE];
int  inc = 0;

void *func(void *arg)
{
  
  int newsize = 100 * (10 - *((int *)arg));
  int vetor[newsize];
  int i, x, y, aux;


  
  for ( i=0; i < newsize; i++)
	vetor[i] = (int) srand % 100 + 1; // cria vetor aleatório com valores entre 1 e 100.

      x = 0;
      y = 0;
      aux = 0;      
  
  // ordena através de bubble sort.
  
  for( x = 0; x < newsize; x++ )
  {
    for( y = x + 1; y < newsize; y++ ) 
    {
     if ( vetor[x] > vetor[y] )
      {
         aux = vetor[x];
         vetor[x] = vetor[y];
         vetor[y] = aux;
	if (x % 50 == 0) // a cada múltiplo de 50 ordenações, o programa cede o seu lugar ao próximo.
		cyield();
	else 
		continue;
      }
    }
  }


 // fim da ordenação
  
  // exibe elementos ordenados   
  printf("%d Elementos ordenados (Crescente):", newsize);
  
  for( x = 0; x < newsize; x++ )
  {
    printf(" %d -  %d ",x,vetor[x]); // exibe o vetor ordenado
  } 
	
}


int main(int argc, char *argv[]) {
    int i, pid[MAX_THR];

    printf("Teste");
    for (i = 0; i < MAX_THR; i++) {
        pid[i] = ccreate(&func, (void *) &i, 0); // a primeira função é a que deve levar mais tempo para terminar.
       if ( pid[i] == -1) {
          printf("ERRO: criação de thread!\n");
          exit(-1);
       }
     }

    for (i = 0; i < MAX_THR; i++) 
         cjoin(pid[i]);
      
    printf("\nConcluida ordenacao dos vetores...\n");
    return NULL;
    exit(0);
}

