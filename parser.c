#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include "parser.h"
#include "zmienne.h"
#include "wektor.h"
#include "qm.h"







void delete(leksem l)
{
	if (l.typ == ZMIENNA)
		free(l.zmienna.nazwa);
}


struct __wezel;
typedef struct __wezel {
	leksem klucz;
	struct __wezel *lewy;
	struct __wezel *prawy;
} Wezel_t;





int ile_wektorow;



Wezel_t* Nowy_Wezel(leksem_type typ)
{
	Wezel_t *ptr;

	ptr = (Wezel_t*)malloc(sizeof(Wezel_t));
	ptr->lewy = NULL;
	ptr->prawy = NULL;
	ptr->klucz.typ = typ;

	return ptr;
}


leksem getsym(FILE *f)
{
	char znak;
	leksem ret;
	char nazwa[NAZWA_MAXLEN];
	char *p;

	while (EOF != (znak = getc(f)) && isspace(znak))
		;

	if (EOF == znak) {
		if (ferror(f))
			Blad(1, "getc()");
		ret.typ = KONIEC;
	} else {

		switch (znak) {
			case '(':
			case ')':
			case '[':
			case ']':
			case '{':
			case '}':
			case '\'':
			case '*':
			case '+':
				/*
				 * Jeden z operatorów?
				 */
				ret.typ = ZNAK;
				ret.znak.c = znak;
				break;
			default:

				/*
				 * Liczba binarna?
				 */

				if (znak == '0' || znak == '1') {
					ret.typ = LICZBA;
					ret.liczba.l = znak - '0';

					while (EOF != (znak = getc(f))) {
						if (znak == '0' || znak == '1') {
							ret.liczba.l <<= 1;
							ret.liczba.l += znak - '0';
						} else {
							ungetc(znak, f);
							break;
						}
					}
					if (ferror(f))
						Blad(1, "getc()");
					break;
				}

				/*
				 * Nazwa zmiennej?
				 */

				if (!isalpha(znak)) {
					ret.typ = NIEZNANY;
					ret.znak.c = znak;
					break;
				}

				ret.typ = ZMIENNA;
				p = nazwa;
				*p++ = znak;
				while (EOF != (znak = getc(f)) && p < &nazwa[NAZWA_MAXLEN - 1]) {
					if (!isalnum(znak)) {
						ungetc(znak, f);
						break;
					}
					*p++ = znak;
				}
				*p = '\0';
				ret.zmienna.nazwa = strdup(nazwa);
				if (ferror(f))
					Blad(1, "getc()");
		}
	}

	return ret;
}


static char* leksem2str(leksem Symbol)
{
	static char buf[30];

	switch (Symbol.typ) {
		case LICZBA:
			snprintf(buf, sizeof(buf), "%d", Symbol.liczba.l);
			break;
		case ZNAK:
		case NIEZNANY:
			snprintf(buf, sizeof(buf), "%c", Symbol.znak.c);
			break;
		case ZMIENNA:
			snprintf(buf, sizeof(buf), "%s", Symbol.zmienna.nazwa);
			break;
		case KONIEC:
			strncpy(buf, "Koniec napisu", sizeof(buf) - 1);
			break;
	}

	buf[sizeof(buf) - 1] = '\0';

	return buf;
}


static FILE *Wejscie;

int Czy_Znak(leksem *Symbol, char znak)
{
	return (Symbol->typ == ZNAK && Symbol->znak.c == znak);
}


Wezel_t* Analizuj(leksem *Symbol);


Wezel_t* Nieterm_CZYN(leksem *Symbol)
{
	Wezel_t *op, *arg;
	Wezel_t *ret;

	if (Symbol->typ == ZNAK)
		switch (Symbol->znak.c) {
			case '(':
				*Symbol = getsym(Wejscie);
				ret = arg = Analizuj(Symbol);
				if (!Czy_Znak(Symbol, ')'))
					Blad(-10, "Nieprawid³owa sk³adnia.\n"
							"Wczytany symbol: %s\n"
							"Oczekiwano zamkniêcia nawiasu ')'.\n", leksem2str(*Symbol));
				*Symbol = getsym(Wejscie);
				break;
			case '[':
				*Symbol = getsym(Wejscie);
				ret = arg = Analizuj(Symbol);
				if (!Czy_Znak(Symbol, ']'))
					Blad(-10, "Nieprawid³owa sk³adnia.\n"
							"Wczytany symbol: %s\n"
							"Oczekiwano zamkniêcia nawiasu ']'.\n", leksem2str(*Symbol));
				*Symbol = getsym(Wejscie);
				break;
			case '{':
				*Symbol = getsym(Wejscie);
				ret = arg = Analizuj(Symbol);
				if (!Czy_Znak(Symbol, '}'))
					Blad(-10, "Nieprawid³owa sk³adnia.\n"
							"Wczytany symbol: %s\n"
							"Oczekiwano zamkniêcia nawiasu '}'.\n", leksem2str(*Symbol));
				*Symbol = getsym(Wejscie);
				break;
			default:
				Blad(-10, "Nieprawid³owa sk³adnia.\n"
						"Wczytany symbol: %s\n"
						"Oczekiwano liczby, zmiennej lub nawiasu.\n", leksem2str(*Symbol));
		}
	else if (Symbol->typ == ZMIENNA) {
		ret = arg = Nowy_Wezel(ZMIENNA);
		arg->klucz.zmienna.nazwa = Symbol->zmienna.nazwa;
		dodaj_zmienna(Symbol->zmienna.nazwa);

		*Symbol = getsym(Wejscie);
	}
	else if (Symbol->typ == LICZBA) {
		if (Symbol->liczba.l != 0 && Symbol->liczba.l != 1)
			Blad(-10, "Nieprawid³owa sk³adnia.\n"
					"Wczytany symbol: %s\n"
					"Mo¿na u¿ywaæ tylko warto¶ci 0 lub 1.\n", leksem2str(*Symbol));
		ret = arg = Nowy_Wezel(LICZBA);
		arg->klucz.liczba.l = Symbol->liczba.l;

		*Symbol = getsym(Wejscie);
	}
	else
		Blad(-10, "Nieprawid³owa sk³adnia.\n"
				"Wczytany symbol: %s\n"
				"Oczekiwano liczby, zmiennej lub nawiasu.\n", leksem2str(*Symbol));

	while (Czy_Znak(Symbol, '\'')) {
		ret = op = Nowy_Wezel(ZNAK);
		op->klucz.znak.c = '\'';
		op->lewy = arg;
		arg = op;
		*Symbol = getsym(Wejscie);
	}

	return ret;
}

Wezel_t* Nieterm_SKL(leksem *Symbol) {
	Wezel_t *czyn1, *op, *czyn2;
	Wezel_t *ret;

	ret = czyn1 = Nieterm_CZYN(Symbol);

	while (Czy_Znak(Symbol, '*') || Czy_Znak(Symbol, '(')
			|| Czy_Znak(Symbol, '[') || Czy_Znak(Symbol, '{')
			|| Symbol->typ == LICZBA || Symbol->typ == ZMIENNA) {
		if (Czy_Znak(Symbol, '*'))
			*Symbol = getsym(Wejscie);

		ret = op = Nowy_Wezel(ZNAK);
		op->klucz.znak.c = '*';

		czyn2 = Nieterm_CZYN(Symbol);

		op->lewy = czyn1;
		op->prawy = czyn2;

		czyn1 = op;
	}

	return ret;
}

Wezel_t* Analizuj(leksem *Symbol) {
	Wezel_t *skl1, *op, *skl2;
	Wezel_t *ret;

	ret = skl1 = Nieterm_SKL(Symbol);

	while (Czy_Znak(Symbol, '+')) {

		ret = op = Nowy_Wezel(ZNAK);
		op->klucz.znak.c = '+';

		*Symbol = getsym(Wejscie);
		skl2 = Nieterm_SKL(Symbol);

		op->lewy = skl1;
		op->prawy = skl2;
		skl1 = op;
	}

	return ret;
}


void Zapisz_Drzewo(FILE *PLIK, Wezel_t *p)
{
	if (p == NULL)
		return;

	switch (p->klucz.typ) {
		case LICZBA:
			fprintf(PLIK, " %d \n", p->klucz.liczba.l);
			break;
		case ZMIENNA:
			fprintf(PLIK, " %s \n", p->klucz.zmienna.nazwa);
			break;
		case ZNAK:
			fprintf(PLIK, " %c \n", p->klucz.znak.c);
			break;
		default:
			break;
	}

	if (p->lewy && p->prawy) {
		fprintf(PLIK, "TN\n");
		Zapisz_Drzewo(PLIK, p->lewy);
		Zapisz_Drzewo(PLIK, p->prawy);
	}
	else if (p->lewy) {
		fprintf(PLIK, "T\n");
		Zapisz_Drzewo(PLIK, p->lewy);
	}
	else if (p->prawy) {
		fprintf(PLIK, "N\n");
		Zapisz_Drzewo(PLIK, p->prawy);
	}
	else
		fprintf(PLIK, "-\n");
}

char **Tabela_Prawdy;


static int
Oblicz(Wezel_t *W)
{
	int ret;

	switch (W->klucz.typ) {
		case LICZBA:
			if (W->klucz.liczba.l)
				ret = 1;
			else
				ret = 0;
			break;
		case ZNAK:
			switch (W->klucz.znak.c) {
				case '\'':
					ret =  !Oblicz(W->lewy);
					break;
				case '*':
					ret =  Oblicz(W->lewy) && Oblicz(W->prawy);
					break;
				case '+':
					ret =  Oblicz(W->lewy) || Oblicz(W->prawy);
					break;
				default:
					ABORT();
			}
			break;
		case ZMIENNA:
			ret = wartosc_zmiennej(W->klucz.zmienna.nazwa);
			break;
		default:
			ABORT();
	}

	return ret;
}


static void
Wariacja(Wezel_t *W)
{
	char buf[MAX_ZMIENNYCH + 1] = {'\0'};
	Zmienna_t *z1;
	char *p;

	if (Oblicz(W)) {

		p = buf;
		z1 = Zmienne;

		while (p < &p[sizeof(buf) - 1] && z1) {
			*p++ = z1->wartosc ? '1' : '0';
			z1 = z1->nast;
		}
		*p = '\0';

		wektory_prawdziwe = dodaj_wektor(wektory_prawdziwe, ustaw_wektor(buf));
		ilosc_prawdziwych++;
	}
}


void Ustaw_wektory_Prawdziwe(Zmienna_t *Z, Wezel_t *W)
{
	if (ile_zmiennych == 0)
		return;

	Z->wartosc = 0;
	if (Z->nast)
		Ustaw_wektory_Prawdziwe(Z->nast, W);
	else
		Wariacja(W);
	Z->wartosc = 1;
	if (Z->nast)
		Ustaw_wektory_Prawdziwe(Z->nast, W);
	else
		Wariacja(W);
}


void Pobiez_Wzor(FILE *F)
{
	leksem Symbol;
	FILE *LOGPLIK;
	Wezel_t *Korzen;

	Wejscie = F;
	Symbol = getsym(Wejscie);
	Korzen = Analizuj(&Symbol);

	if ((LOGPLIK = fopen("dane.dat", "w"))) {
		Zapisz_Drzewo(LOGPLIK, Korzen);
		fclose(LOGPLIK);
	}

	if (Symbol.typ != KONIEC)
		Blad(-10, "Nieprawid³owa sk³adnia.\n"
				"Wczytany symbol: %s\n"
				"Spodziewano siê koñca napisu.\n", leksem2str(Symbol));
	putchar('\n');

	if (ile_zmiennych == 0) {
		int wynik;

		wynik = Oblicz(Korzen);
		printf("========[ Wzór ]========\n");
		if (wynik)
			printf("1\n");
		else
			printf("0\n");
		exit(EXIT_SUCCESS);
	}

	Ustaw_wektory_Prawdziwe(Zmienne, Korzen);

#if 0
	{
		lista *l = wektory_prawdziwe;
		printf("Wektory prawdziwe:\n");
		for (; l; l = l->nast)
			pokaz_wektor(l->w);
	}
#endif

}


#define MAX_VARS 8

/*
void Tryb_Tabela_Prawdy()
{
	int n;
	char **wartosci;
	char buf[33] = {'\0'};
	char *p;

	fscanf(stdin, "%d", &ile_wektorow);

	wartosci = malloc(ile_wektorow * sizeof(*wartosci));


	Tabela_Prawdy = malloc(ile_wektorow * sizeof(char *));
	for (n = 0; n < ile_wektorow; ++n) {
		fscanf(stdin, "%32s", buf);
		for (p = buf; *p; ++p)
			if (*p != '0' && *p != '1')
				Blad(-3, "Z³a sk³adnia\n");
		printf("%llu\n", strtoll(buf, NULL, 2));
		wartosci[strtoll(buf, NULL, 2)]

	}
}
*/

#if 0
int main(int argc, const char **argv)
{
	leksem symbol;
	char tryb;
	Wezel_t *Korzen;
	FILE *PLIK;
	int n;

	ile_zmiennych = 0;
	ile_wektorow = 0;

	Wejscie = stdin;

	if (1 != fscanf(stdin, "%c", &tryb) || ((tryb = tolower(tryb)) != 'w' && tryb != 't'))
		Blad(-2, "z³y tryb\n");

	if (tryb == 't') {
//		Tryb_Tabela_Prawdy();

	} else if (tryb == 'w') {
		printf("Wzór\n");
		symbol = getsym(Wejscie);
		Korzen = Analizuj(&symbol);
		PLIK = fopen("dane.dat", "w");
		Zapisz_Drzewo(PLIK, Korzen);
		fclose(PLIK);
		if (symbol.typ != KONIEC)
			Blad(-3, "Z³a sk³adnia\n");
		printf("Przeanalizowano ok.\n");


	}
	printf("zmiennych: %d\n", ile_zmiennych);



	/*
	 * Tabelki do algorytmu q-m dobrze bêdzie trzymaæ w tablicy char[][]
	 */



#if 0
	while (!feof(stdin)) {
		symbol = getsym(stdin);
		
			switch (symbol.typ) {
				case ZNAK:
					printf("ZNAK %c\n", symbol.znak.c);
					break;
				case LICZBA:
					printf("LICZBA %d\n", symbol.liczba.l);
					break;
				case ZMIENNA:
					printf("ZMIENNA %s\n", symbol.zmienna.nazwa);
					break;
				case NIEZNANY:
					printf("NIEZNANY SYMBOL %c\n", symbol.znak.c);
					break;
				case KONIEC:
					printf("KONIEC NAPISU\n");
					break;
			}
		}

		delete(symbol);
	}
#endif

	printf("\n");


	exit(EXIT_SUCCESS);
}
#endif
