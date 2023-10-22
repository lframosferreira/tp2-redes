#---------------------------------------------------------------------
# Arquivo	: Makefile
# Conteúdo	: Compilação do Trabalho Prático 2 - Disciplina de Redes de Computadores
# Autor		: Luís Felipe Ramos Ferreira - Matrícula: 2019022553
#---------------------------------------------------------------------
# Opções	: make - compila tudo e gera os binários
#		: make clean - remove objetos e executável
#---------------------------------------------------------------------

CC = gcc
LIBS = -lm
BIN = bin
OBJ = obj
HDRS = common.h topic.h
CFLAGS = -Wall

COMMON = $(OBJ)/common.o
TOPIC = $(OBJ)/topic.o
SERVER = $(BIN)/server
CLIENT = $(BIN)/client

all: $(COMMON) $(TOPIC) $(SERVER) $(CLIENT) 

$(COMMON): common.c $(HDRS)
	test -d $(OBJ) || mkdir $(OBJ)
	test -d $(BIN) || mkdir $(BIN)
	$(CC) $(CFLAGS) -c -o $(OBJ)/common.o common.c

$(TOPIC): topic.c $(HDRS)
	$(CC) $(CFLAGS) -c -o $(OBJ)/TOPIC.o topic.c

$(SERVER): server.c $(HDRS)
	$(CC) $(CFLAGS) server.c -o $(BIN)/server $(OBJ)/* $(LIBS)

$(CLIENT): client.c $(HDRS)
	$(CC) $(CFLAGS) client.c -o $(BIN)/client $(OBJ)/common.o $(LIBS)

clean:
	rm $(SERVER) $(CLIENT)
	rm $(OBJ)/*

testeserver:
	./bin/server v4 51511

testeserver6:
	./bin/server v6 51511

testeclient:
	./bin/client 127.0.0.1 51511

testeclient6:
	./bin/client ::1 51511