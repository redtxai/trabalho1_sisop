/* 
 * test_ordenamento.c: recebe um inteiro, cria uma array de valores aleatorios com este inteiro e ordena através do bubble sort. O objetivo é testar inits, yelds e suspends durante a execução de funções demoradas e rápidas. A cada 100 ordenamentos a função libera intencionalmente seu recurso.
 */

#include	"../include/support.h"
#include	"../include/cthread.h"
#include	<stdio.h>
#include	<stdlib.h>
#include 	<time.h>
#include 	"../src/cthread.c"



int vetor[MAX_SIZE];
int  inc = 0;

void *func(void *arg){
{
  // criar função que demora para rodar. 
 
}


int main(int argc, char *argv[]) {

	// criar três threads.
	// Dar yieald na primeira.
	// dar join na segunda, dependendo da tarceira.
	// finalizar terceira, segunda e primeira.
	
	// testar funções de resume e suspend.

	// como testar semáforo e outras funções?


}

