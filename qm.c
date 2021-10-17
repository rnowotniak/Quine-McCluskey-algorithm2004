#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include "qm.h"
#include "parser.h"
#include "wektor.h"
#include "tabela.h"
#include "algorytm.h"
#include "zmienne.h"


int ile_zmiennych;
int ile_wektorow;
FILE *Wejscie;


void Blad(int ecode, const char *fmt, ...)
{
	va_list ap;

	fprintf(stderr, "B³±d: ");
	if (ecode <= 0) {
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
		exit(-ecode);
	} else {
		perror(fmt);
		exit(ecode);
	}
}


int Wczytaj_Dane(FILE *F) {
	leksem Symbol;

	ilosc_prawdziwych = 0;

	Symbol = getsym(Wejscie);

	if (Symbol.typ != ZMIENNA ||
			(strcasecmp(Symbol.zmienna.nazwa, "t") && strcasecmp(Symbol.zmienna.nazwa, "w")))
		Blad(-2, "Z³a sk³adnia - oczekiwano ,,t'' lub ,,w''\n");

	if (!strcasecmp(Symbol.zmienna.nazwa, "t")) {
		Wczytaj_Tabele_Prawdy(F);
	}
	else if (!strcasecmp(Symbol.zmienna.nazwa, "w")) {
		Pobiez_Wzor(F);
	}

	if (ilosc_prawdziwych == 0) {
		printf("========[ Wzór ]========\n");
		printf("0\n");
		exit(EXIT_SUCCESS);
	}

	printf("========[ Ilo¶æ zmiennych: %d ]========\n", ile_zmiennych);
	printf("========[ Ilo¶æ prawdziwych: %d ]========\n", ilosc_prawdziwych);

	return 0;
}

lista *wektory_prawdziwe = NULL,
		*wektory_obojetne = NULL;

Zmienna_t *Zmienne = NULL;

#ifndef TEST

int main(int argc, const char **argv)
{

	Wejscie = stdin;

	close(2);
	dup(1);

	if (Wczytaj_Dane(stdin))
		Blad(-1, "Nie uda³o siê wczytaæ danych\n");

//	pokaz_zmienne();

	Algorytm();

	printf("\n");

	exit(EXIT_SUCCESS);
}

#endif

