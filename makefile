CFLAGS = -Wall -g 

all: vina++

vina++: vina++.o archive.o caminho.o diretorio.o lista.o operacoes.o
	gcc -o vina++ vina++.o archive.o caminho.o diretorio.o lista.o operacoes.o

vina++.o: vina++.c
	gcc -c vina++.c $(CFLAGS)

archive.o: archive.c archive.h
	gcc -c archive.c $(CFLAGS)

caminho.o: caminho.c caminho.h
	gcc -c caminho.c $(CFLAGS)

diretorio.o: diretorio.c diretorio.h
	gcc -c diretorio.c $(CFLAGS)

lista.o: lista.c lista.h
	gcc -c lista.c $(CFLAGS)

operacoes.o: operacoes.c operacoes.h
	gcc -c operacoes.c $(CFLAGS)

clean:
	-rm *.o

purge:
	-rm *.o vina++