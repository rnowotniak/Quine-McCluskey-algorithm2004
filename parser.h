#ifndef __HAVE_PARSER_H
#define __HAVE_PARSER_H
#include <inttypes.h>

#define bool int
#define NAZWA_MAXLEN 20

typedef enum _l_typ { LICZBA, ZNAK, ZMIENNA, NIEZNANY, KONIEC } leksem_type;

extern int ile_zmiennych;

struct _leks_zmienna {
	leksem_type typ;
	char *nazwa;
};

struct _leks_liczba {
	leksem_type typ;
	uint32_t l;
};

struct _leks_znak {
	leksem_type typ;
	char c;
};

typedef union _leksem {
	leksem_type typ;
	struct _leks_zmienna zmienna;
	struct _leks_liczba liczba;
	struct _leks_znak znak;
} leksem;


leksem getsym(FILE *f);
void Pobiez_Wzor(FILE *F);

#endif
