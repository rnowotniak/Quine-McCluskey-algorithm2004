#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include "qm.h"
#include "zmienne.h"
#include "wektor.h"

/*
 * Dwa wektory nale¿± do REL1, gdy pierwszy ma mniejsz±
 * liczbê jedynek ni¿ drugi
 *
 *
 * Dwa wektory nale¿± do REL2, gdy maj± my¶lnik
 * na tej samej pozycji i ró¿ni± siê jednym innym polem.
 * XXX: (co najwy¿ej?)
 *
 */

lista* Utworz_Tabelke1(lista *prawdziwe, lista *obojetne)
{
	lista *ret, *p1, *p2, *t;

	/*
	 * Utworzenie nowej listy przez kopiowanie elementów
	 * z obu list
	 */

	/*
	 * XXX: Co w sytuacji gdy jedna z list jest NULL?
	 */


	p1 = prawdziwe;

	ret = p2 = malloc(sizeof(lista));
	p2->w = prawdziwe->w;
	p2->nast = NULL;

	for (p1 = p1->nast; p1; p1 = p1->nast) {
		p2 = ret;

		if (RELACJA1(p1->w, p2->w)) {
			p2 = malloc(sizeof(lista));
			p2->w = p1->w;
			p2->nast = ret;
			ret = p2;
			continue;
		}

		while (p2->nast && !RELACJA1(p1->w, p2->nast->w))
			p2 = p2->nast;
		if (p2->nast) {
			t = p2->nast;
			p2->nast = malloc(sizeof(lista));
			p2->nast->nast = t;
			p2->nast->w = p1->w;
		} else {
			p2->nast = malloc(sizeof(lista));
			p2->nast->w = p1->w;
			p2->nast->nast = NULL;
		}

	}

	for (p1 = obojetne; p1; p1 = p1->nast) {
		p2 = ret;

		if (RELACJA1(p1->w, p2->w)) {
			p2 = malloc(sizeof(lista));
			p2->w = p1->w;
			p2->nast = ret;
			ret = p2;
			continue;
		}

		while (p2->nast && !RELACJA1(p1->w, p2->nast->w))
			p2 = p2->nast;
		if (p2->nast) {
			t = p2->nast;
			p2->nast = malloc(sizeof(lista));
			p2->nast->nast = t;
			p2->nast->w = p1->w;
		} else {
			p2->nast = malloc(sizeof(lista));
			p2->nast->w = p1->w;
			p2->nast->nast = NULL;
		}

	}

#if 0
	printf("Uporz±dkowana tabelka:\n");
	for (p1 = ret; p1; p1 = p1->nast)
		pokaz_wektor(p1->w);
#endif

	return ret;

}

lista *grupy[20];
int ilosc_grup;
int ilosc_implikantow;
int ilosc_prawdziwych;


int Utworz_Grupy(lista *Tabelka)
{
	lista *p;
	int ret = 0;

	if (!Tabelka)
		return 0;

	grupy[0] = Tabelka;
	ret = 1;

	for (p = Tabelka; p->nast; p = p->nast)
		if (RELACJA1(p->w, p->nast->w))
			grupy[ret++] = p->nast;

	return ret;
}

void Pokaz_Grupy(lista *grupy[], int ilosc)
{
	lista *p;
	int n;

	if (ilosc == 0)
		return;

	for (n = 0; n < ilosc - 1; ++n) {
		for (p = grupy[n]; p != grupy[n + 1]; p = p->nast)
			pokaz_wektor(p->w);
		printf("=======\n");
	}
	for (p = grupy[n]; p; p = p->nast)
			pokaz_wektor(p->w);

}


void Usun_Powtorki(lista *T)
{
	lista *p, *tmp;

	if (!T || !T->nast)
		return;

	for (p = T; p && p->nast; p = p->nast) {
		while (p->nast && p->w == p->nast->w) {
			tmp = p->nast;
			p->nast = p->nast->nast;
			free(tmp);
			ilosc_implikantow--;
		}
	}
}



lista* implikanty = NULL;

lista* Cykl1(lista *grupy[], int ilosc)
{
	int n;
	lista *p, *r;
	lista *ret = NULL;
	wektor_t sklejony;

	/*
	 * Sklejanie odpowiednich wektorów z s±siednich grup
	 */

	for (n = 0; n < ilosc - 1; ++n) {
		for (p = grupy[n]; p && p != grupy[n + 1]; p = p->nast) {
			for (r = grupy[n + 1]; r && r != grupy[n + 2]; r = r->nast) {
				if (RELACJA2(p->w, r->w)) {
					sklejony = sklej_wektory(p->w, r->w);
					ret = dodaj_wektor(ret, sklejony);
					p->w = oznacz_wektor(p->w);
					r->w = oznacz_wektor(r->w);
				}
			}
		}
	}

	/*
	 * Przej¶cie przez wszystkie wektory w pocz±tkowej tabelce
	 * i wyszukanie niezaznaczonych.
	 */

	for (p = grupy[0]; p; p = p->nast)
		if (!czy_zaznaczony(p->w)) {
			implikanty = dodaj_wektor(implikanty, p->w);
			ilosc_implikantow++;
		}

#if 0
	printf("Nowa tabelka:\n");
	for (p = ret; p; p = p->nast)
		pokaz_wektor(p->w);
#endif

	if (!implikanty)
		printf("Wszystkie wektory ulega³y sklejaniu\n");
	else {
		printf("Niesklejone wektory:\n");
		for (p = implikanty; p; p = p->nast)
			pokaz_wektor(p->w);
	}

	return ret;
}

void Pokaz_Implikanty(void)
{
	lista *p;

	printf("========[ Ostateczne implikanty ]========\n");
	for (p = implikanty; p; p = p->nast)
		pokaz_wektor(p->w);
}


typedef enum {PUSTE = 0, WYBRANE = 1, KOLKO = 2, KWADRAT = 4} pole_t;

void Pokaz_Tabelke2(pole_t **T)
{
	int w, k;
	lista *praw;
	lista *imp;

	printf("       ");
	for (praw = wektory_prawdziwe, k = 0; k < ilosc_prawdziwych && praw; ++k, praw = praw->nast) {
		printf("|%7s", wek2str(praw->w));
	}
	printf("\n");

	for (imp = implikanty, w = 0; w < ilosc_implikantow && imp; ++w, imp = imp->nast) {
		printf("%7s", wek2str(imp->w));
		for (k = 0; k < ilosc_prawdziwych; ++k)
			printf("|%7d", T[w][k]);
		printf("\n");
	}

}

lista* Tabelka2(lista *implikanty, lista *wektory_prawdziwe)
{
	int w, k;
	int ile, old, poz, jest_kolko;
	int zaznaczone_kolumny = 0;
	lista *imp, *skl, *wybor;

	pole_t **TAB2;

	TAB2 = malloc(ilosc_implikantow * sizeof(*TAB2));
	for (w = 0; w < ilosc_implikantow; ++w)
		TAB2[w] = malloc(ilosc_prawdziwych * sizeof(**TAB2));

	/*
	 * Wstêpne wype³nienie tabelki
	 */

	for (w = 0, imp = implikanty; imp; w++, imp = imp->nast)
		for (k = 0, skl = wektory_prawdziwe; skl; k++, skl = skl->nast)
			if (Wektor_Pasuje(skl->w, imp->w))
				TAB2[w][k] = WYBRANE;
			else
				TAB2[w][k] = PUSTE;

	/*
	 * Przejechanie przez wszystkie kolumny w poszukiwaniu takich
	 * która mia³yby tylko jedno wybrane pole.
	 * Zaznaczenie tego pola.
	 */

	for (k = 0, skl = wektory_prawdziwe; k < ilosc_prawdziwych && skl; k++, skl = skl->nast) {
		for (ile = 0, w = 0, imp = implikanty; w < ilosc_implikantow && imp; w++, imp = imp->nast)
			if (TAB2[w][k] == WYBRANE) {
				ile++;
				wybor = imp;
				poz = w;
			}
		if (ile == 1) {
			TAB2[poz][k] |= KOLKO;
			wybor->w = oznacz_wektor(wybor->w);
			skl->w = oznacz_wektor(skl->w);
			zaznaczone_kolumny++;
		}
	}

	/*
	 * Przejechanie przez wszystkie wiersze w poszukiwaniu takiego,
	 * w którym jest zaznaczone kó³ko.
	 * Zaznaczenie wszystkich wybranych pól w tym wierszu kwadratem.
	 */

	for (w = 0; w < ilosc_implikantow; ++w) {
		for (k = 0, jest_kolko = 0; !jest_kolko && k < ilosc_prawdziwych; ++k)
			if (TAB2[w][k] & KOLKO)
				jest_kolko = 1;
		if (jest_kolko)
			for (k = 0, skl = wektory_prawdziwe; k < ilosc_prawdziwych && skl; k++, skl = skl->nast)
				if (TAB2[w][k] == WYBRANE) {
					TAB2[w][k] |= KWADRAT;
					if (!czy_zaznaczony(skl->w)) {
						skl->w = oznacz_wektor(skl->w);
						zaznaczone_kolumny++;
					}
				}
	}

	/*
	 * Wykonanie koñcówki zadania
	 */

	while (zaznaczone_kolumny != ilosc_prawdziwych) {

		w = 0;
		imp = implikanty;

		old = 0;
		poz = 0;
		wybor = implikanty;
		while (w < ilosc_implikantow && imp) {

			if (!czy_zaznaczony(imp->w)) {
				k = 0;
				skl = wektory_prawdziwe;
				ile = 0;
				while (k < ilosc_prawdziwych && skl) {
					if (TAB2[w][k] == WYBRANE && !czy_zaznaczony(skl->w))
						ile++;
					k++;
					skl = skl->nast;
				}

				if (ile > old) {
					old = ile;
					wybor = imp;
					poz = w;
				}

			}

			w++;
			imp = imp->nast;
		}

		wybor->w = oznacz_wektor(wybor->w);

		k = 0;
		skl = wektory_prawdziwe;
		while (k < ilosc_prawdziwych && skl) {
			if (TAB2[poz][k] == WYBRANE && !czy_zaznaczony(skl->w)) {
				skl->w = oznacz_wektor(skl->w);
				zaznaczone_kolumny++;
			}

			k++;
			skl = skl->nast;
		}

	}

	Pokaz_Tabelke2(TAB2);

	/*
	 * XXX: Przejechanie przez wszystkie implikanty i stworzenie nowej listy
	 * sk³adaj±cej siê z zaznaczonych.
	 */

	for (wybor = NULL, imp = implikanty; imp; imp = imp->nast)
		if (czy_zaznaczony(imp->w))
			wybor = dodaj_wektor(wybor, imp->w);

	return wybor;
}


void Pokaz_Rozwiazanie(lista *l)
{
	lista *imp;
	char *p;
	int czy_jest_skladnik, czy_jest_czynnik;
	Zmienna_t *Z;

	if (!l)
		return;

	putchar('\n');
	printf("========[ Rozwi±zanie ]========\n");

	for (imp = l; imp; imp = imp->nast)
		pokaz_wektor(imp->w);

	putchar('\n');
	printf("========[ Wzór ]========\n");

	for (imp = l; imp; imp = imp->nast) {
		p = wek2str(imp->w);
		Z = Zmienne;
		czy_jest_skladnik = 0;
		czy_jest_czynnik = 0;
		while (Z && p && *p != ' ') {

			if (*p == '0') {
				if (czy_jest_czynnik)
					printf("*");
				printf("%s'", Z->nazwa);
				czy_jest_skladnik = 1;
				czy_jest_czynnik = 1;
			} else if (*p == '1') {
				if (czy_jest_czynnik)
					printf("*");
				printf("%s", Z->nazwa);
				czy_jest_skladnik = 1;
				czy_jest_czynnik = 1;
			}

			Z = Z->nast;
			p++;
		}
		if (!czy_jest_skladnik)
			printf("1 ");
		if (imp->nast)
			printf(" + ");
	}

}


void Algorytm(void)
{
	lista *tabelka = NULL;
	lista *wynik = NULL;
	int numer_cyklu = 1;

	ilosc_implikantow = 0;

	if (!wektory_prawdziwe) {
		implikanty = dodaj_wektor(implikanty, ustaw_wektor("0"));
		ilosc_implikantow = 1;
	}
	else {

		tabelka = Utworz_Tabelke1(wektory_prawdziwe, wektory_obojetne);

		for (numer_cyklu = 1; tabelka; numer_cyklu++) {
			putchar('\n');
			printf("========[ Cykl numer %d ]========\n", numer_cyklu);

			ilosc_grup = Utworz_Grupy(tabelka);
			printf("Ilo¶æ grup: %d\n", ilosc_grup);

			Pokaz_Grupy(grupy, ilosc_grup);

			tabelka = Cykl1(grupy, ilosc_grup);
		}

		Usun_Powtorki(implikanty);
	}

	putchar('\n');
	Pokaz_Implikanty();

	putchar('\n');
	printf("========[ Tabelka 2 ]========\n");

	wynik = Tabelka2(implikanty, wektory_prawdziwe);

	Pokaz_Rozwiazanie(wynik);

}

