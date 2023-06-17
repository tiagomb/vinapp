CFLAGS = -Wall -g 

all: vina++

vina++: vina++.o archive.o caminho.o diretorio.o lista.o operacoes.o
	gcc -o vina++ vina++.o archive.o caminho.o diretorio.o lista.o operacoes.o

vina++.o: ./src/vina++.c
	gcc -c ./src/vina++.c $(CFLAGS)

archive.o: ./src/archive.c ./src/archive.h
	gcc -c ./src/archive.c $(CFLAGS)

caminho.o: ./src/caminho.c ./src/caminho.h
	gcc -c ./src/caminho.c $(CFLAGS)

diretorio.o: ./src/diretorio.c ./src/diretorio.h
	gcc -c ./src/diretorio.c $(CFLAGS)

lista.o: ./src/lista.c ./src/lista.h
	gcc -c ./src/lista.c $(CFLAGS)

operacoes.o: ./src/operacoes.c ./src/operacoes.h
	gcc -c ./src/operacoes.c $(CFLAGS)

clean:
	-rm *.o

purge:
	-rm *.o vina++