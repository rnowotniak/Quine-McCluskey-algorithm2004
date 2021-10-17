CC = gcc
CFLAGS = -Wall -g

OBJS = algorytm.o parser.o qm.o tabela.o wektor.o zmienne.o

qm: $(OBJS)
	$(CC) $(CFLAGS) -o qm algorytm.o parser.o qm.o tabela.o wektor.o zmienne.o

algorytm.o: algorytm.c qm.h wektor.h zmienne.h
	$(CC) $(CFLAGS) -c algorytm.c

parser.o: parser.c parser.h zmienne.h wektor.h qm.h
	$(CC) $(CFLAGS) -c parser.c

qm.o: qm.c qm.h parser.h wektor.h tabela.h algorytm.h zmienne.h
	$(CC) $(CFLAGS) -c qm.c

tabela.o: tabela.c wektor.h zmienne.h parser.h qm.h
	$(CC) $(CFLAGS) -c tabela.c

wektor.o: wektor.c wektor.h qm.h
	$(CC) $(CFLAGS) -c wektor.c

zmienne.o: zmienne.c zmienne.h qm.h
	$(CC) $(CFLAGS) -c zmienne.c

clean:
	-rm qm core $(OBJS)

install: qm
	cp qm ../bin
	chmod 755 ../bin/qm

scp:
	scp *.c *.h Makefile ox1:html/qm/source

tgz: clean $(SRCS) Makefile
	cd .. ; tar czvf qm.tar.gz source
	chmod 644 ../qm.tar.gz

.PHONY: clean scp

