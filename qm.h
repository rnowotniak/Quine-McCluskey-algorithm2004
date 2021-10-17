#ifndef __HAVE_QM_H
#define __HAVE_QM_H
#include "wektor.h"

#define MAX_ZMIENNYCH 19
#define ABORT() Blad(-1, "Co¶ trzeba poprawiæ w kodzie. "\
		"linia %d, plik %s\n", __LINE__, __FILE__)


extern int ile_zmiennych;
extern int ile_wektorow;
extern int ilosc_prawdziwych;
extern FILE *Wejscie;

extern lista *wektory_prawdziwe;
extern lista *wektory_obojetne;


void Blad(int ecode, const char *fmt, ...);

#endif
