#include "../include/cthread.h"
#include "../include/cdata.h"
#include <ucontext.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ERROR_CODE -1
#define SUCCESS_CODE 0

#define TRUE_CODE 1
#define FALSE_CODE 0

FILA2 ready;
FILA2 blocked;
FILA2 ready_suspended;
FILA2 blocked_suspended;
TCB_t *running_thread = NULL;  // Executando

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
        Txai Mostardeiro Potier - 00252858\n
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

int cresume(int tid) {
    int returncode = ERROR_CODE
    if (running_thread != NULL && running_thread->tid != tid) {
        if (checkisblockedsuspended(tid)) {
            TCB_t *value = (TCB_t *) getthreadfromblockedsuspended(tid);
            addthreadinblockedsuspended(value);
            removethreadinblockedsuspended(value);
            returncode = SUCCESS_CODE;
        }
        if (checkisreadysuspended(tid)) {
            TCB_t *value = (TCB_t *) getthreadfromreadysuspended(tid);
            addthreadinreadysuspended(value);
            removethreadinreadysuspended(value);
            returncode = SUCCESS_CODE;
        }
    }
    return returncode;
}

int csuspend(int tid) {
    int returncode = ERROR_CODE
    if (running_thread != NULL && running_thread->tid != tid) {
        if (checkisblocked(tid)) {
            TCB_t *value = (TCB_t *) getthreadfromblocked(tid);
            addthreadinblocked(value);
            removethreadinblocked(value);
            returncode = SUCCESS_CODE;
        }
        if (checkisready(tid)) {
            TCB_t *value = (TCB_t *) getthreadfromready(tid);
            addthreadinready(value);
            removethreadinready(value);
            returncode = SUCCESS_CODE;
        }
    }
    return returncode;
}


/***************************************************************************
***** Aqui começam as funções de suporte para csuspend() e cresume() *******
***************************************************************************/


// verifica se uma determinada thread está na fila de bloqueados
int checkisblocked (int tid) {
    int returncode = FALSE_CODE;
    if (FirstFila2(blocked) == 0) {
        do {
            TCB_t *value = (TCB_t *)GetAtIteratorFila2(blocked);
            if (value != NULL)
                if (value->tid == tid) {
                    returncode = TRUE_CODE;
                }
        } while (NextFila2(blocked) == 0);
    }
    return returncode;
}

// verifica se uma determinada thread está na fila de aptos
int checkisready (int tid) {
    int returncode = FALSE_CODE;
    if (FirstFila2(ready) == 0) {
        do {
            TCB_t *value = (TCB_t *)GetAtIteratorFila2(ready);
            if (value != NULL)
                if (value->tid == tid) {
                    returncode = TRUE_CODE;
                }
        } while (NextFila2(ready) == 0);
    }
    return returncode;
}

// verifica se uma determinada thread está na fila de aptos suspensos
int checkisreadysuspended (int tid) {
    int returncode = FALSE_CODE;
    if (FirstFila2(ready_suspended) == 0) {
        do {
            TCB_t *value = (TCB_t *)GetAtIteratorFila2(ready_suspended);
            if (value != NULL)
                if (value->tid == tid) {
                    returncode = TRUE_CODE;
                }
        } while (NextFila2(ready_suspended) == 0);
    }
    return returncode;
}

// verifica se uma determinada thread está na fila de bloqueados suspensos
int checkisblockedsuspended (int tid) {
    int returncode = FALSE_CODE;
    if (FirstFila2(blocked_suspended) == 0) {
        do {
            TCB_t *value = (TCB_t *)GetAtIteratorFila2(blocked_suspended);
            if (value != NULL)
                if (value->tid == tid) {
                    returncode = TRUE_CODE;
                }
        } while (NextFila2(blocked_suspended) == 0);
    }
    return returncode;
}

// pega uma determinada thread através de seu tid que encontra-se na fila de bloqueados
TCB_t *getthreadfromblocked(int tid) {
    TCB_t *value;
    do {
        value = (TCB_t *)GetAtIteratorFila2(blocked);
        if (value != NULL)
            if (value->tid == tid) {
                break;
            }
    } while (NextFila2(blocked_suspended) == 0);
    return value;
}

// pega uma determinada thread através de seu tid que encontra-se na fila de aptos
TCB_t *getthreadfromready(int tid) {
    TCB_t *value;
    do {
        value = (TCB_t *)GetAtIteratorFila2(ready);
        if (value != NULL)
            if (value->tid == tid) {
                break;
            }
    } while (NextFila2(ready) == 0);
    return value;
}


/***************************************************************************
********* Aqui começam as funções para adição e remoção nas filas **********
***************************************************************************/

// blocked_suspended
// adiciona uma determinada thread na fila de bloqueados suspensos
int addthreadinblockedsuspended(TCB_t *thread) {
    return AppendFila2(blocked_suspended, thread);
}

// remove uma determinada thread da fila de bloqueados suspensos
int removethreadinblockedsuspended(TCB_t *thread) {
    return DeleteAtIteratorFila2(blocked_suspended, thread);
}

// ready_suspended
// adiciona uma determinada thread na fila de aptos suspensos
int addthreadinreadysuspended(TCB_t *thread) {
    return AppendFila2(ready_suspende, thread);
}

// remove uma determinada thread da fila de aptos suspensos
int removethreadinreadysuspended(TCB_t *thread) {
    return DeleteAtIteratorFila2(ready_suspended, thread);
}


// blocked
// adiciona uma determinada thread na fila de bloqueados
int addthreadinblocked(TCB_t *thread) {
    return AppendFila2(blocked, thread);
}

// remove uma determinada thread da fila de bloqueados
int removethreadinblocked(TCB_t *thread) {
    return DeleteAtIteratorFila2(blocked, thread);
}

//ready
// adiciona uma determinada thread na fila de aptos
int addthreadinready(TCB_t *thread) {
    return AppendFila2(ready, thread);
}

// remove uma determinada thread da fila de aptos
int removethreadinready(TCB_t *thread) {
    return DeleteAtIteratorFila2(ready, thread);
}