#ifndef ARGUMENT_PARSER_H_
#define ARGUMENT_PARSER_H_

#include "parg_def.h"
#include "parg_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct parg
{
	int restriction;
	int value_restriction;
	const char *name;
	void *callback_data;
	void (*callback)(void *, const char *, const char *, int);
	struct parg *childs[CHAR_MAX];
} parg;

PARG_EXPORT parg *parg_alloc();
PARG_EXPORT void parg_free(parg *);
PARG_EXPORT void parg_add_callback(parg *, const char *, void (*callback)(void *, const char *, const char *, int), void *, int, int);
PARG_EXPORT void parg_parse(parg *, int, char **);

#ifdef __cplusplus
}
#endif

#endif /* ARGUMENT_PARSER_H_ */ 