#include "../include/cthread.h"
#include "../include/cdata.h"
#include <ucontext.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ERROR_CODE -1
#define SUCCESS_CODE 0

FILA2 ready;
FILA2 blocked;
FILA2 ready_suspended;
FILA2 blocked_suspended;

/*
 * init: Inicializa as variáveis globais compartilhadas pelas funções da biblioteca
 *
 * Retorno:
 *  Quando executada corretamente: retorna 0 (zero).
 *  Caso contrário, retorna um valor negativo.
 */
int init() {
    if (!initialized_globals) {
        initialized_globals = true;

        // inicializar a thread main e colocar em executando.
        if (init_main_thread() != SUCCESS_CODE) {
            return ERROR_CODE;
        }

        // inicializar o contexto de finalizacao de thread.
        if (init_ending_ctx() != SUCCESS_CODE) {
            return ERROR_CODE;
        }

        // Tamanho de cada struct de fila
        size_t queue_size = sizeof(struct sFila2);

        // Inicializa as diversas filas de threads
        int i;
        for (i = 0; i < 4; ++i) {
            ready[i] = malloc(queue_size);
            CreateFila2(ready[i]);
        }
        blocked_join = (FILA2 *) malloc(sizeof(FILA2));

        if (CreateFila2(blocked_join) != SUCCESS_CODE) {
            return ERROR_CODE;
        }

        blocked_semaphor = (FILA2 *) malloc(sizeof(FILA2));
        if (CreateFila2(blocked_semaphor) != SUCCESS_CODE) {
            return ERROR_CODE;
        }

        return SUCCESS_CODE;
    }

    return SUCCESS_CODE;
}

/*
 * generateThreadId: Gera um id único para uma nova thread
 *
 * Retorno:
 *  Inteiro contendo um id único de thread
 */
int currentThreadId = 0;

int generateThreadId()
{
	return ++currentThreadId;
}

/*
 * cidentify: Printa na tela a identificação do grupo
 *
 * Parâmetros:
 *  name: ponteiro para uma área de memória onde deve ser escrito um string
 *  que contém os nomes dos componentes do grupo e seus números de cartão.
 *  Deve ser uma linha por componente.
 *
 *  size: quantidade máxima de caracteres que podem ser copiados para o
 *  string de identificação dos componentes do grupo.
 *
 * Retorno:
 *  Quando executada corretamente: retorna 0 (zero).
 *  Caso contrário, retorna um valor negativo.
 */
int cidentify (char *name, int size)
{
	char *team = 
		"Gabriel Tiburski Júnior - 00229713\n
		Txai - XXXXXXXX\n
		Vinicius - XXXXXXXX";

	if(strncpy(name, team, size) == 0) {
		return SUCCESS_CODE;
	}
	else {
		return ERROR_CODE;
	}
}

/*
 * ccreate: Cria uma nova thread.
 *
 * Parâmetros:
 *  start: ponteiro para a função que a thread executará.
 *
 *  arg: um parâmetro que pode ser passado para a thread na sua criação.
 *  (Obs.: é um único parâmetro. Se for necessário passar mais de um valor
 *  deve-se empregar um ponteiro para uma struct)
 *
 *  prio: NÃO utilizado neste semestre, deve ser sempre zero.
 *
 * Retorno:
 *  Quando executada corretamente: retorna um valor positivo, que representa
 *  o identificador da thread criada.
 *  Caso contrário, retorna um valor negativo.
 */
int ccreate(void *(*start)(void *), void *arg, int prio)
{
    init();

    TCB_t *newThread = (TCB_t *)malloc(sizeof(TCB_t));

    newThread->tid = generateThreadId();
    newThread->state = PROCST_CRIACAO;
    newThread->prio = 0;

    getcontext(&(newThread->context));

    newThread->context.uc_link = ending_ctx;
    newThread->context.uc_stack.ss_sp = malloc(SIGSTKSZ)
    newThread->context.uc_stack.ss_size = SIGSTKSZ;

    /*if (newThread->context.uc_stack.ss_sp == NULL) {
        return ERROR_CODE;
    }*/
    
    makecontext(&(newThread->context), (void (*)(void))start, 1, arg);

    ready_push(newThread);

    return newThread->tid;
}