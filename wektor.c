#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include "wektor.h"
#include "qm.h"


static uint32_t POTEGI_3[] = {
	1uL,
	3uL,
	9uL,
	27uL,
	81uL,
	243uL,
	729uL,
	2187uL,
	6561uL,
	19683uL,
	59049uL,
	177147uL,
	531441uL,
	1594323uL,
	4782969uL,
	14348907uL,
	43046721uL,
	129140163uL,
	387420489uL,
	1162261467uL
};



int RELACJA1(wektor_t w1, wektor_t w2)
{
	uint32_t wartosc1 = w1, wartosc2 = w2;
	int jedynki1 = 0, jedynki2 = 0;
	int n;

	wartosc1 %= POTEGI_3[19];
	wartosc2 %= POTEGI_3[19];

	for (n = 18; n >=0; --n) {
		if (wartosc1 / POTEGI_3[n] == 1)
			jedynki1++;
		if (wartosc2 / POTEGI_3[n] == 1)
			jedynki2++;
		wartosc1 %= POTEGI_3[n];
		wartosc2 %= POTEGI_3[n];
	}

	return jedynki1 < jedynki2;
}

int RELACJA2(wektor_t w1, wektor_t w2)
{
	int n;
	enum {NIE, TAK, BRAK} pole1, pole2;
	int roznice = 0;

	w1 %= POTEGI_3[19];
	w2 %= POTEGI_3[19];

	for (n = 18; n >=0; --n) {
		pole1 = w1 / POTEGI_3[n];
		pole2 = w2 / POTEGI_3[n];

		if (pole1 == BRAK || pole2 == BRAK) {
			if (pole1 != BRAK || pole2 != BRAK)
				return 0;
		} else
			if (pole1 != pole2) {
				if (roznice == 1)
					return 0;
				else
					roznice++;
			}

		w1 %= POTEGI_3[n];
		w2 %= POTEGI_3[n];
	}

	return 1;
}

void pokaz_wektor(wektor_t W)
{
//	printf("[%010d] ", W);

	printf("%s\n", wek2str(W));
}


char* wek2str(wektor_t W)
{
	static char BUFOR[MAX_ZMIENNYCH + 3];
	uint32_t wartosc = W;
	int width, n;
	char *p;

	if (ile_zmiennych <= MAX_ZMIENNYCH)
		width = ile_zmiennych;
	else
		width = MAX_ZMIENNYCH;

	wartosc %= POTEGI_3[19];

	for (p = (char*)BUFOR, n = width - 1; n >=0; --n) {
		switch (wartosc / POTEGI_3[n]) {
			case 0:
				*p++ = '0';
				break;
			case 1:
				*p++ = '1';
				break;
			case 2:
				*p++ = '-';
				break;
		}
		wartosc %= POTEGI_3[n];
	}
	if (W / POTEGI_3[19]) {
		*p++ = ' ';
		*p++ = 'X';
	}
	*p = '\0';
	return (char*)BUFOR;
}

wektor_t ustaw_wektor(const char *str)
{
	wektor_t ret = 0;
	int n;
	const char *p;

	for (n = 0, p = &str[strlen(str) - 1]; p >= str; --p, ++n) {
		switch (*p) {
			case '0':
				break;
			case '1':
				ret += POTEGI_3[n];
				break;
			case '-':
				ret += 2uL * POTEGI_3[n];
				break;
		}
	}

	return ret;
}


wektor_t sklej_wektory(wektor_t w1, wektor_t w2)
{
	int n;
	wektor_t ret = 0;

	w1 %= POTEGI_3[19];
	w2 %= POTEGI_3[19];

	for (n = 18; n >=0; --n) {
		if (w1 / POTEGI_3[n] == w2 / POTEGI_3[n])
			ret += w1 / POTEGI_3[n] * POTEGI_3[n];
		else
			ret += 2 * POTEGI_3[n];

		w1 %= POTEGI_3[n];
		w2 %= POTEGI_3[n];
	}


	return ret;
}


wektor_t oznacz_wektor(wektor_t w)
{
	wektor_t ret = w;

	if (!(ret / POTEGI_3[19]))
		ret += POTEGI_3[19];

	return ret;
}


int czy_zaznaczony(wektor_t w)
{
	return w / POTEGI_3[19];
}


/*
 * Dodaje wektor do listy, ale nie powoduje pojawiania siê duplikatów
 */
lista* dodaj_wektor(lista *l, wektor_t w)
{
	lista *p;

	if (!l) {
		l = malloc(sizeof(lista));
		l->w = w;
		l->nast = NULL;
		return l;
	}

	p = l;
	while (p->nast && p->w != w)
		p = p->nast;

	if (p->w == w)
		return l;

	p->nast = malloc(sizeof(lista));
	p = p->nast;
	p->w = w;
	p->nast = NULL;

	return l;
}


/*
 * Sprawdza, czy w1 pasuje do w2.
 * Wektor w1 pasuje do w2, gddy na wszystkich polach:
 * 	zgadzaj± siê warto¶ci
 * 	lub
 * 	na w2 jest -
 */
int Wektor_Pasuje(wektor_t w1, wektor_t w2)
{
	int n;

	for (n = 18; n >= 0; --n) {
		if ((w1 / POTEGI_3[n] != w2 / POTEGI_3[n])
			&& (w2 / POTEGI_3[n] != 2))
			return 0;
		w1 %= POTEGI_3[n];
		w2 %= POTEGI_3[n];
	}

	return 1;
}


#ifdef TEST

int main(int argc, const char **argv)
{
	wektor_t w1;

	w1 = ustaw_wektor("11-0");
	pokaz_wektor(w1);

	printf("\n");

	exit(EXIT_SUCCESS);
}

#endif

