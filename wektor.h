#ifndef __HAVE_WEKTOR_H
#define __HAVE_WEKTOR_H
#include <inttypes.h>

typedef uint32_t wektor_t;
char* wek2str(wektor_t W);
void pokaz_wektor(wektor_t W);
wektor_t ustaw_wektor(const char *str);

struct __lista_t;
typedef struct __lista_t {
	wektor_t w;
	struct __lista_t *nast;
} lista;

lista* dodaj_wektor(lista *l, wektor_t w);
wektor_t sklej_wektory(wektor_t w1, wektor_t w2);

int RELACJA1(wektor_t w1, wektor_t w2);
int RELACJA2(wektor_t w1, wektor_t w2);
int Wektor_Pasuje(wektor_t w1, wektor_t w2);

wektor_t oznacz_wektor(wektor_t w);
int czy_zaznaczony(wektor_t w);

#endif
