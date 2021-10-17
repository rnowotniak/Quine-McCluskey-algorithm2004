#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "zmienne.h"
#include "qm.h"


void dodaj_zmienna(const char *nazwa)
{
	Zmienna_t *p;

	if (Zmienne == NULL) {
		Zmienne = malloc(sizeof(Zmienna_t));
		Zmienne->nazwa = (char*)nazwa;
		Zmienne->wartosc = 0;
		Zmienne->nast = NULL;
		ile_zmiennych = 1;
		return;
	}

	p = Zmienne;
	if (!strcasecmp(p->nazwa, nazwa))
		return;
	while (p->nast && strcasecmp(p->nast->nazwa, nazwa))
		p = p->nast;

	if (p->nast)
		return;

	p->nast = malloc(sizeof(Zmienna_t));
	p->nast->nazwa = (char*)nazwa;
	p->nast->wartosc = 0;
	p->nast->nast = NULL;
	ile_zmiennych++;

}


void pokaz_zmienne(void)
{
	Zmienna_t *p;

	if (!Zmienne) {
		printf("Nie ma ¿adnych zmiennych");
		return;
	}

	for (p = Zmienne; p; p = p->nast)
		printf("%5s|", p->nazwa);
	putchar('\n');

	for (p = Zmienne; p; p = p->nast)
		printf("%5d|", p->wartosc);
	putchar('\n');

}


int wartosc_zmiennej(const char* nazwa)
{
	Zmienna_t *p;

	if (!Zmienne)
		ABORT();

	for (p = Zmienne; p; p = p->nast)
		if (!strcasecmp(nazwa, p->nazwa))
			break;

	if (!p)
		ABORT();

	return p->wartosc;
}

