/* 
 * test_functs: testa as primitivas ccreate, cyield, cjoin, cresume e ccreate.
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
	//for(i=0; i < 10000000; i++){
			
	//}
	sleep(5);
	printf("Finalizando função\n");	
 	return 0;
}


int main(int argc, char *argv[]) {
	
	int id0, id1, id2, id3;
	int i=0;
	
	id0 = ccreate(func, (void *)&i, 0);
	i++;
	id1 = ccreate(func, (void *)&i, 0);
	i++;
	id2 = ccreate(func, (void *)&i, 0);
	i++;
	id3 = ccreate(func, (void *)&i, 0);
	csuspend(id3);  // id3 entra em estado suspenso.
	sleep(1);
	cresume(id3); // tira id3 do estado suspenso.
	sleep(3);
	
	
	cyield(); // id0 cede sua vez.

	

	printf("Finalizando o esquema\n"); 

	return 0;	

}

