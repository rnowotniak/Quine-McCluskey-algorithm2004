#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include "wektor.h"
#include "zmienne.h"
#include "parser.h"
#include "qm.h"



int skip_space(FILE *F)
{
	char znak;
	while ((znak = getc(F)) != EOF)
		if (znak != '\n' && znak != ' ' && znak != '\t')
			break;
	return znak;
}


void Wczytaj_Tabele_Prawdy(FILE *F)
{
	int n;
	char wektor[21] = {'\0'};
	char *p;
	lista *lista1 = NULL, *lista2 = NULL;
	lista *l;
	leksem Symbol;

	/*
	 * Wczytanie nazw zmiennych
	 */

	ile_zmiennych = 0;
	Symbol = getsym(F);
	while (Symbol.typ == ZMIENNA) {
//		printf("zmienna: %s\n", Symbol.zmienna.nazwa);
		dodaj_zmienna(Symbol.zmienna.nazwa);
		Symbol = getsym(F);
	}

	if (ile_zmiennych == 0)
		Blad(-4, "Przy podawaniu tabeli nie mo¿e byæ zero zmiennych\n");

	/*
	 * Wczytanie wektorów
	 */

	while (Symbol.typ != KONIEC) {
		wektor[0] = '\0';
		p = wektor;
		n = 1;
		do {
			if (Symbol.typ != LICZBA || (Symbol.liczba.l != 0 && Symbol.liczba.l != 1))
				printf("B³±d()\n");

			*p++ = '0' + Symbol.liczba.l;
			Symbol = getsym(F);
			if (Symbol.typ == KONIEC)
				printf("EOF\n");
		} while (n++ < ile_zmiennych);

		if (Symbol.typ == LICZBA && Symbol.liczba.l == 1) {
#if 0
			printf("Dodajê %s do wektorów true\n", wektor);
#endif
			lista1 = dodaj_wektor(lista1, ustaw_wektor(wektor));
			ilosc_prawdziwych++;
		}
		else if (Symbol.typ == LICZBA && Symbol.liczba.l == 0) {
#if 0
			printf("Wektora %s nie dodajê nigdzie\n", wektor);
#endif
			;
		}
		else if (Symbol.typ == NIEZNANY && Symbol.znak.c == '?') {
#if 0
			printf("Dodajê %s do wektorów niewiadomych\n", wektor);
#endif
			lista2 = dodaj_wektor(lista2, ustaw_wektor(wektor));
		}
		Symbol = getsym(F);
	}

	for (l = lista1, ilosc_prawdziwych = 0; l; l = l->nast)
		ilosc_prawdziwych++;

	wektory_prawdziwe = lista1;
	wektory_obojetne = lista2;

}



#if 0
int main(int argc, const char **argv)
{


	Wczytaj_Tabele_Prawdy(stdin);

	printf("\n");


	exit(EXIT_SUCCESS);
}

#endif
