#
# Makefile de EXEMPLO
#
# OBRIGATÓRIO ter uma regra "all" para geração da biblioteca e de uma
# regra "clean" para remover todos os objetos gerados.
#
# É NECESSARIO ADAPTAR ESSE ARQUIVO de makefile para suas necessidades.
#  1. Cuidado com a regra "clean" para não apagar o "support.o"
#
# OBSERVAR que as variáveis de ambiente consideram que o Makefile está no diretótio "cthread"
#

CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src

all: libcthread.a

libcthread.a: $(BIN_DIR)/cthread.o $(BIN_DIR)/support.o
	ar crs $(LIB_DIR)/libcthread.a $(BIN_DIR)/cthread.o $(BIN_DIR)/support.o

$(BIN_DIR)/cthread.o: $(SRC_DIR)/cthread.c
	$(CC) -c -I$(INC_DIR) $(SRC_DIR)/cthread.c -Wall -ls -o $(BIN_DIR)/cthread.o

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/cthread.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~

#DEFAULT
#all: regra1 regra2 regran
#
#regra1: #dependências para a regra1
#	$(CC) -o $(BIN_DIR)regra1 $(SRC_DIR)regra1.c -Wall
#
#regra2: #dependências para a regra2
#	$(CC) -o $(BIN_DIR)regra2 $(SRC_DIR)regra2.c -Wall
#
#regran: #dependências para a regran
#	$(CC) -o $(BIN_DIR)regran $(SRC_DIR)regran.c -Wall
#
#clean:
#	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~
