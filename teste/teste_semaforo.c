/* 
 * test_semaforo.c: Função demorada (longo for) testa as primitivas ccreate, cwait e csignal.
 */

#include	"../include/support.h"
#include	"../include/cthread.h"
#include	<stdio.h>
#include	<stdlib.h>
#include 	<time.h>
#include 	"../src/cthread.c"



void *func(void *arg)
{
	int i = 0;
	printf("iniciando função\n");
  // criar função que demora para rodar.
	for(i=0; i < 10000; i++){
	
			
	}
	printf("Finalizando função \n");	
 
}


int main(int argc, char *argv[]) {
	
	int id0, id1, id2, id3;
	int i=0;

	// inicializar struct do semáforo.
	csem_t recurso;
	
	// começar a rodar funcoes demoradas
	id0 = ccreate(func, (void *)&i, 0);
	i++;
	id1 = ccreate(func, (void *)&i, 0);
	i++;
	id2 = ccreate(func, (void *)&i, 0);
	i++;
	id3 = ccreate(func, (void *)&i, 0);	
	
	
	// incluir testes do semáforo.
	csem_init(&recurso, 1);
	
	cwait(&recurso); // id0 pega o recurso.
	cwait(&recurso);
	csignal(&recurso); 
	cwait(&recurso);
	cwait(&recurso);
	csignal(&recurso);
	csignal(&recurso);
	csignal(&recurso);
		
}

