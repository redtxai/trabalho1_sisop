#
# Makefile para programas exemplo
#

CC=gcc
LIB_DIR=../lib

all: exemplo mandel series teste_functs teste_ordenamento teste_semaforo teste_vetor

exemplo: exemplo.c $(LIB_DIR)/libcthread.a
	$(CC) -o exemplo exemplo.c -L$(LIB_DIR) -lcthread -Wall

mandel: mandel.c $(LIB_DIR)/libcthread.a
	$(CC) -o mandel mandel.c -L$(LIB_DIR) -lcthread -Wall

series: series.c $(LIB_DIR)/libcthread.a
	$(CC) -o series series.c -L$(LIB_DIR) -lcthread -Wall

teste_functs: teste_functs.c $(LIB_DIR)/libcthread.a
	$(CC) -o teste_functs teste_functs.c -L$(LIB_DIR) -lcthread -Wall

teste_ordenamento: teste_ordenamento.c $(LIB_DIR)/libcthread.a
	$(CC) -o teste_ordenamento teste_ordenamento.c -L$(LIB_DIR) -lcthread -Wall

teste_semaforo: teste_semaforo.c $(LIB_DIR)/libcthread.a
	$(CC) -o teste_semaforo teste_semaforo.c -L$(LIB_DIR) -lcthread -Wall

teste_vetor: teste_vetor.c $(LIB_DIR)/libcthread.a
	$(CC) -o teste_vetor teste_vetor.c -L$(LIB_DIR) -lcthread -Wall

clean:
	rm -rf exemplo mandel series teste_functs teste_ordenamento teste_semaforo teste_vetor *.o *~
