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

TCB_t *runningThread = NULL;  // Thread executando atualmente
ucontext_t *finisherContext = NULL;  // Contexto que será carregado ao fim de uma thread

FILA2 *ready; // Fila de aptos
FILA2 *blocked; // Fila de bloqueados
FILA2 *readySuspended; // Fila de aptos suspensos
FILA2 *blockedSuspended; // Fila de bloqueados suspensos

int nextUniqueThreadId = 0; // Próximo ID de thread que pode ser usado na criação

int isLibraryInitialized = 0;

/*
 * generateThreadId: Gera um id único para uma nova thread
 *
 * Retorno:
 *  Inteiro contendo um id único de thread
 */
int generateThreadId()
{
    return nextUniqueThreadId++;
}

/*
 * GetThreadFromFila2: Busca por uma thread numa fila e retorna seu valor
 *
 * Parâmetros:
 *  tid: id da thread a ser buscada
 *  pFila: ponteiro para uma FILA2 qualquer
 *
 * Retorno:
 *  Retorna um ponteiro para thread quando encontrá-la na fila.
 *  Quando não encontrar, retorna NULL;
 */
TCB_t *GetThreadFromFila2(int tid, FILA2 *fila)
{
    TCB_t *thread = NULL;

    FirstFila2(fila);

    do {
        thread = (TCB_t *)GetAtIteratorFila2(fila);
        if (thread != NULL && thread->tid == tid)
            return thread;
    } while (NextFila2(fila) == 0);

    return NULL;
}

/*
 * RemoveThreadFromFila2: Remove uma thread de uma fila
 *
 * Parâmetros:
 *  tid: id da thread a ser removida
 *  pFila: ponteiro para uma FILA2 qualquer
 *
 * Retorno:
 *  Retorna SUCCESS_CODE caso tenha encontrado e removido a thread com sucesso.
 *  Caso contrário, retorna ERROR_CODE.
 */
int RemoveThreadFromFila2(int tid, FILA2 *fila)
{
    TCB_t *thread = NULL;

    if(FirstFila2(fila) != 0)
        return ERROR_CODE;

    do {
        thread = (TCB_t *)GetAtIteratorFila2(fila);
        if (thread != NULL && thread->tid == tid) {
            if(DeleteAtIteratorFila2(fila) == 0)
                return SUCCESS_CODE;
            else
                return ERROR_CODE;
        }
    } while(NextFila2(fila) == 0);

    return ERROR_CODE;
}

/*
 * DequeueThreadInFila2: Remove a thread na primeira posição da fila e retorna
 * seu valor.
 *
 * Parâmetros:
 *  pFila: ponteiro para uma FILA2 qualquer
 *
 * Retorno:
 *  Retorna um ponteiro para thread quando executada corretamente.
 *  Caso a fila esteja vazia ou com erro retorna NULL (@todo verificar quando der erros)
 */
TCB_t *DequeueThreadInFila2(FILA2 *fila)
{
    TCB_t *thread = NULL;

    FirstFila2(fila);
    thread = (TCB_t *)GetAtIteratorFila2(fila);
    DeleteAtIteratorFila2(fila);

    return thread;
}

/*
 * EnqueueThreadInFila2: Insere uma thread no final da fila
 *
 * Parâmetros:
 *  thread: ponteiro para uma thread qualquer
 *
 *  pFila: ponteiro para uma FILA2 qualquer
 *
 * Retorno:
 *  Retorna SUCCESS_CODE quando executada corretamente.
 *  Caso contrário, retorna ERROR_CODE.
 */
int EnqueueThreadInFila2(TCB_t *thread, FILA2 *fila)
{
    if(AppendFila2(fila, thread) == 0)
        return SUCCESS_CODE;
    else
        return ERROR_CODE;
}

/*
 * makeReady: Troca o estado de uma thread para Ready/ReadSuspended
 *
 * Parâmetros:
 *  thread: ponteiro para uma thread qualquer
 *
 *  pFila: ponteiro para uma FILA2 qualquer
 *
 * Retorno:
 *  Retorna SUCCESS_CODE quando executada corretamente.
 *  Caso contrário, retorna ERROR_CODE.
 */
int makeReady(int tid) {
    thread = GetThreadFromFila2(tid, blockedSuspended);
    if(thread != NULL) {
        RemoveThreadFromFila2(tid, blockedSuspended);
        thread->state = PROCST_APTO_SUS;
        EnqueueThreadInFila2(thread, readySuspended);
        return SUCCESS_CODE;
    }

    thread = GetThreadFromFila2(tid, blocked);
    if(thread != NULL) {
        RemoveThreadFromFila2(tid, blocked);
        thread->state = PROCST_APTO;
        EnqueueThreadInFila2(thread, ready);
        return SUCCESS_CODE;
    }
}


/*
 * swapContext: Função que faz a troca de contexto
 *
 * Retorno:
 *  Retorna SUCCESS_CODE quando executada corretamente.
 *  Caso contrário, retorna ERROR_CODE.
 */
int swapContext()
{
    if (runningThread != NULL) {
        getcontext(&(runningThread->context));
    }

    runningThread = DequeueThreadInFila2(ready);

    if (runningThread == NULL) {
        return ERROR_CODE;
    }

    runningThread->state = PROCST_EXEC;
    setcontext(&(runningThread->context));

    return SUCCESS_CODE;
}

/*
 * onEndThread: Função a ser executada quando uma thread termina (exceto main)
 */
void onEndThread()
{
    //@todo provavelmente tem mais coisas pra fazer aqui
    swapContext();
}

/*
 * initFila: Inicializa uma fila qualquer
 *
 * Parâmetros:
 *  fila: ponteiro para uma FILA2 qualquer
 *
 * Retorno:
 *  Retorna SUCCESS_CODE quando executada corretamente.
 *  Caso contrário, retorna ERROR_CODE.
 */
int initFila(FILA2 *fila)
{
    fila = (FILA2 *) malloc(sizeof(FILA2));

    if (CreateFila2(fila) != 0) {
        return ERROR_CODE;
    }
    return SUCCESS_CODE;
}

/*
 * initMainThread: Inicializa a thread principal (main) e a coloca no estado
 * "executando"
 *
 * Retorno:
 *  Retorna SUCCESS_CODE quando executada corretamente.
 *  Caso contrário, retorna ERROR_CODE.
 */
int initMainThread()
{
    TCB_t *mainThread = (TCB_t *) malloc(sizeof(TCB_t));

    mainThread->tid = generateThreadId();
    mainThread->prio = 0;

    // Alocação de memória para a pilha da main
    (mainThread->context).uc_stack.ss_sp = malloc(SIGSTKSZ);
    (mainThread->context).uc_stack.ss_size = SIGSTKSZ;

    // Quando a thread main finaliza, o programa deve terminar. Logo,
    // não deve ser atribuído um callback
    (mainThread->context).uc_link = NULL;

    // A thread main é a primeira a ser executada, logo vai direto
    // para o estado "executando"
    mainThread->state = PROCST_EXEC;
    runningThread = mainThread;

    return SUCCESS_CODE;
}

/*
 * initFinisherContext: Inicializa o contexto que será executado ao
 * fim das threads
 *
 * Retorno:
 *  Retorna SUCCESS_CODE quando executada corretamente.
 *  Caso contrário, retorna ERROR_CODE.
 */
int initFinisherContext()
{
    finisherContext = (ucontext_t *) malloc(sizeof(ucontext_t));

    // Inicialização de contexto ocorreu corretamente?
    if(getcontext(finisherContext) != 0) {
        return ERROR_CODE;
    }

    // Alocação de memória para a pilha da main
    finisherContext->uc_stack.ss_sp = malloc(SIGSTKSZ);
    finisherContext->uc_stack.ss_size = SIGSTKSZ;

    // Nada a fazer quando acabar
    finisherContext->uc_link = NULL;

    makecontext(finisherContext, onEndThread, 0);

    return SUCCESS_CODE;
}

/*
 * init: Inicializa todas as variáveis da biblioteca.
 *
 * Retorno:
 *  Retorna SUCCESS_CODE quando executada corretamente.
 *  Caso contrário, retorna ERROR_CODE.
 */
int init()
{
    if (!isLibraryInitialized) {

        int initFilaReturns = initFila(ready);
        initFilaReturns += initFila(blocked);
        initFilaReturns += initFila(readySuspended);
        initFilaReturns += initFila(blockedSuspended);

        if (initFilaReturns != SUCCESS_CODE*4) {
            return ERROR_CODE;
        }

        if (initMainThread() != SUCCESS_CODE) {
            return ERROR_CODE;
        }

        if (initFinisherContext() != SUCCESS_CODE) {
            return ERROR_CODE;
        }

        isLibraryInitialized = TRUE_CODE;
    }

    return SUCCESS_CODE;
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
        "Vinicius Roratto Carvalho - 00160094\n";

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
    newThread->state = PROCST_APTO;
    newThread->prio = 0;

    getcontext(&(newThread->context));

    newThread->context.uc_link = finisherContext;
    newThread->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
    newThread->context.uc_stack.ss_size = SIGSTKSZ;

    if (newThread->context.uc_stack.ss_sp == NULL) {
        return ERROR_CODE;
    }

    makecontext(&(newThread->context), (void (*)(void))start, 1, arg);

    EnqueueThreadInFila2(newThread, ready);

    return newThread->tid;
}

/*
 * cyield: A thread atual libera voluntariamente a CPU (retorna ao estado apto)
 *
 * Retorno:
 *  Quando executada corretamente: retorna 0 (zero)
 *  Caso contrário, retorna um valor negativo.
 */
int cyield(void)
{
    init();

    runningThread->state = PROCST_APTO;
    EnqueueThreadInFila2(runningThread, ready);

    return swapContext();
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
int cresume(int tid)
{
    init();

    TCB_t *thread = NULL;

    thread = GetThreadFromFila2(tid, blockedSuspended);
    if(thread != NULL) {
        RemoveThreadFromFila2(tid, blockedSuspended);
        EnqueueThreadInFila2(thread, blocked);
        return SUCCESS_CODE;
    }

    thread = GetThreadFromFila2(tid, readySuspended);
    if(thread != NULL) {
        RemoveThreadFromFila2(tid, readySuspended);
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
int csuspend(int tid)
{
    init();

    TCB_t *thread = NULL;

    thread = GetThreadFromFila2(tid, blocked);
    if(thread != NULL) {
        RemoveThreadFromFila2(tid, blocked);
        EnqueueThreadInFila2(thread, blockedSuspended);
        return SUCCESS_CODE;
    }

    thread = GetThreadFromFila2(tid, ready);
    if(thread != NULL) {
        RemoveThreadFromFila2(tid, ready);
        EnqueueThreadInFila2(thread, readySuspended);
        return SUCCESS_CODE;
    }

    return ERROR_CODE;
}

/*
 * cjoin: @todo escrever sobre a função
 *
 * Parâmetros:
 *  tid: identificador da thread cujo término está sendo aguardado.
 *
 * Retorno:
 *  Quando executada corretamente: retorna 0 (zero)
 *  Caso contrário, retorna um valor negativo.
 */
int cjoin(int tid)
{
    //@todo
}

/*
 * csem_init: @todo escrever sobre a função
 *
 * Parâmetros:
 *  sem: ponteiro para uma variável do tipo csem_t.
 *  Aponta para uma estrutura de dados que representa a variável semáforo.
 *
 *  count: valor a ser usado na inicialização do semáforo.
 *  Representa a quantidade de recursos controlados pelo semáforo.
 *
 * Retorno:
 *  Quando executada corretamente: retorna 0 (zero)
 *  Caso contrário, retorna um valor negativo.
 */
int csem_init(csem_t *sem, int count)
{
    sem = (csem_t *)malloc(sizeof(csem_t));
    sem->fila = (PFILA2)malloc(sizeof(FILA2));
    sem->count = count;

    CreateFila2(sem->fila);

    if (sem->fila)
        return SUCESSO;
    return ERRO; //caso fila nao tenha sido alocada corretamente
}

/*
 * cwait: Solicita um recurso. 
 * Se o recurso estiver livre, ele é atribuido a thread. 
 * Se recurso estiver ocupado, thread sendo utilizada é bloqueada e colocada na fila do semáforo.
 *
 * Parâmetros:
 *  sem: ponteiro para uma variável do tipo semáforo.
 *
 * Retorno:
 *  Quando executada corretamente: retorna 0 (zero)
 *  Caso contrário, retorna um valor negativo.
 */
int cwait(csem_t *sem)
{
    init();
    // Semáforo nulo ou fila não inicializada retornam erro.
    if ((sem= NULL) || (sem->fila == NULL))
        return ERROR_CODE;
    // Recurso disponível é passado para a thread.
    if (sem-> count > 0) {
        sem->count--;
        return SUCCESS_CODE;
    }
    // recurso sendo utilizado. Colocar em estado bloqueado e na fila do semáforo. 
    else {    
        sem->count--;
        running_thread->state = PROCST_BLOQ;
        AppendFila2(sem->fila, running_thread);
        swapContext(); // verificar funcionamento.
        
    }
return SUCCESS_CODE;


    }

/*
 * csignal: @todo escrever sobre a função
 *
 * Parâmetros:
 *  sem: ponteiro para uma variável do tipo semáforo.
 *
 * Retorno:
 *  Quando executada corretamente: retorna 0 (zero)
 *  Caso contrário, retorna um valor negativo.
 */
int csignal(csem_t *sem)
{
    init();
    // Semáforo nulo ou fila não inicializada retornam erro.
    if ((sem= NULL) || (sem->fila == NULL))
        return ERROR_CODE;

    sem->count++;
    // Fila vazia
    if(FirstFila2(sem->fila) != 0)
        return SUCCESS_CODE;

    TCB_t *thread = DequeueThreadInFila2(sem->fila)

    if (thread == NULL) {
        return ERROR_CODE;
    }

    return makeReady(thread->tid);
}
