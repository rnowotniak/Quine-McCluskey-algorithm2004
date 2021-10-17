#ifndef __HAVE_ZMIENNE_H
#define __HAVE_ZMIENNE_H



struct __zmienna;
typedef struct __zmienna {
	char *nazwa;
	int wartosc;
	struct __zmienna *nast;
} Zmienna_t;


extern Zmienna_t *Zmienne;

void dodaj_zmienna(const char *nazwa);
void pokaz_zmienne(void);
int wartosc_zmiennej(const char* nazwa);


#endif
