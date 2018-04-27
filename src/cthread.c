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

PFILA2 ready;
PFILA2 blocked;
PFILA2 ready_suspended;
PFILA2 blocked_suspended;
TCB_t *running_thread = NULL;  // Executando

int currentThreadId = 0;

/*
 * generateThreadId: Gera um id único para uma nova thread
 *
 * Retorno:
 *  Inteiro contendo um id único de thread
 */
int generateThreadId()
{
    return ++currentThreadId;
}

/*
 * GetThreadFromFila2: Busca por uma thread numa fila e retorna seu valor
 *
 * Parâmetros:
 *  tid: id da thread a ser buscada
 *  pFila: ponteiro para uma PFILA2 qualquer
 *
 * Retorno:
 *  Retorna um ponteiro para thread quando encontrá-la na fila.
 *  Quando não encontrar, retorna NULL;
 */
TCB_t *GetThreadFromFila2(int tid, PFILA2 pFila) {
    TCB_t *thread = NULL;

    FirstFila2(pFila);

    do {
        thread = (TCB_t *)GetAtIteratorFila2(pFila);
        if (thread != NULL && thread->tid == tid)
            return thread;
    } while (NextFila2(pFila) == 0);

    return NULL;
}

/*
 * RemoveThreadFromFila2: Remove uma thread de uma fila
 *
 * Parâmetros:
 *  tid: id da thread a ser removida
 *  pFila: ponteiro para uma PFILA2 qualquer
 *
 * Retorno:
 *  Retorna SUCCESS_CODE caso tenha encontrado e removido a thread com sucesso.
 *  Caso contrário, retorna ERROR_CODE.
 */
int RemoveThreadFromFila2(int tid, PFILA2 pFila) {
    TCB_t *thread = NULL;

    if(FirstFila2(pFila) != 0)
        return ERROR_CODE;

    do {
        thread = (TCB_t *)GetAtIteratorFila2(pFila);
        if (thread != NULL && thread->tid == tid) {
            if(DeleteAtIteratorFila2(pFila) == 0)
                return SUCCESS_CODE;
            else
                return ERROR_CODE;
        }
    } while(NextFila2(pFila) == 0);

    return ERROR_CODE;
}

/*
 * DequeueThreadInFila2: Remove a thread na primeira posição da fila e retorna
 * seu valor.
 *
 * Parâmetros:
 *  pFila: ponteiro para uma PFILA2 qualquer
 *
 * Retorno:
 *  Retorna um ponteiro para thread quando executada corretamente.
 *  Caso a fila esteja vazia ou com erro retorna NULL (@todo verificar quando der erros)
 */
TCB_t *DequeueThreadInFila2(PFILA2 pFila) {
    TCB_t *thread = NULL;

    FirstFila2(pFila);
    thread = (TCB_t *)GetAtIteratorFila2(pFila);
    DeleteAtIteratorFila2(pFila);

    return thread;
}

/*
 * EnqueueThreadInFila2: Insere uma thread no final da fila
 *
 * Parâmetros:
 *  thread: ponteiro para uma thread qualquer
 *
 *  pFila: ponteiro para uma PFILA2 qualquer
 *
 * Retorno:
 *  Retorna SUCCESS_CODE quando executada corretamente.
 *  Caso contrário, retorna ERROR_CODE.
 */
int EnqueueThreadInFila2(TCB_t *thread, PFILA2 pFila) {
    if(AppendFila2(pFila, thread) == 0)
        return SUCCESS_CODE;
    else
        return ERROR_CODE;
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
        "Gabriel Tiburski Júnior - 00229713\n"
        "Txai Mostardeiro Potier - 00252858\n"
        "Vinicius - XXXXXXXX";

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
    return 0;
    //@todo ESTA FUNÇAO AINDA NAO ESTA FUNCIONANDO
    /*
    //init();

    TCB_t *newThread = (TCB_t *)malloc(sizeof(TCB_t));

    newThread->tid = generateThreadId();
    newThread->state = PROCST_CRIACAO;
    newThread->prio = 0;

    getcontext(&(newThread->context));

    //newThread->context.uc_link = ending_ctx; //What to do when finished
    newThread->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
    newThread->context.uc_stack.ss_size = SIGSTKSZ;

    if (newThread->context.uc_stack.ss_sp == NULL) {
        return ERROR_CODE;
    }

    makecontext(&(newThread->context), (void (*)(void))start, 1, arg);

    //ready_push(newThread);

    return newThread->tid;
    */
}

/*
 * cresume: Retira uma thread do estado suspenso.
 * (Apto suspenso ou bloqueado suspenso)
 *
 * Parâmetros:
 *  tid: identificador da thread que terá sua execução retomada.
 *
 * Retorno:
 *  Quando executada corretamente: retorna 0 (zero)
 *  Caso contrário, retorna um valor negativo.
 */
int cresume(int tid) {
    TCB_t *thread = NULL;

    thread = GetThreadFromFila2(tid, blocked_suspended);
    if(thread != NULL) {
        RemoveThreadFromFila2(tid, blocked_suspended);
        EnqueueThreadInFila2(thread, blocked);
        return SUCCESS_CODE;
    }

    thread = GetThreadFromFila2(tid, ready_suspended);
    if(thread != NULL) {
        RemoveThreadFromFila2(tid, ready_suspended);
        EnqueueThreadInFila2(thread, ready);
        return SUCCESS_CODE;
    }

    return ERROR_CODE;
}

/*
 * csuspend: Coloca uma thread no estado suspenso.
 * (Apto suspenso ou bloqueado suspenso)
 *
 * Parâmetros:
 *  tid: identificador da thread a ser suspensa.
 *
 * Retorno:
 *  Quando executada corretamente: retorna 0 (zero)
 *  Caso contrário, retorna um valor negativo.
 */
int csuspend(int tid) {
    TCB_t *thread = NULL;

    thread = GetThreadFromFila2(tid, blocked);
    if(thread != NULL) {
        RemoveThreadFromFila2(tid, blocked);
        EnqueueThreadInFila2(thread, blocked_suspended);
        return SUCCESS_CODE;
    }

    thread = GetThreadFromFila2(tid, ready);
    if(thread != NULL) {
        RemoveThreadFromFila2(tid, ready);
        EnqueueThreadInFila2(thread, ready_suspended);
        return SUCCESS_CODE;
    }

    return ERROR_CODE;
}
