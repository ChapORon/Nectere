#ifndef ARGUMENT_PARSER_H_
#define ARGUMENT_PARSER_H_

#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct parg
{
	int restriction;
	void (*vcallback)(const char *);
	void (*ecallback)();
	struct parg *childs[CHAR_MAX];
} parg;

parg *parg_alloc();
void parg_free(parg *);
void parg_add_vcallback(parg *, const char *, void (*callback)(const char *), int);
void parg_add_ecallback(parg *, const char *, void (*callback)());
void parg_parse(parg *, int, char **);

#ifdef __cplusplus
}
#endif

#endif /* ARGUMENT_PARSER_H_ */ 