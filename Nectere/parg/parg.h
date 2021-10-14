#ifndef ARGUMENT_PARSER_H_
#define ARGUMENT_PARSER_H_

#include "parg_def.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct parg
{
	int restriction;
	int value_restriction;
	const char *name;
	void (*callback)(const char *, const char *, int);
	struct parg *childs[CHAR_MAX];
} parg;

parg *parg_alloc();
void parg_free(parg *);
void parg_add_callback(parg *, const char *, void (*callback)(const char *, const char *, int), int, int);
void parg_parse(parg *, int, char **);

#ifdef __cplusplus
}
#endif

#endif /* ARGUMENT_PARSER_H_ */ 