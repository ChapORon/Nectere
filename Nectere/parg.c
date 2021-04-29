#include "parg.h"

#include <stdlib.h>

static parg *get_argument_child(parg *root, const char *str)
{
	char c = str[0];
	if (c != '\0')
	{
		parg *child = root->childs[c];
		if (child != NULL)
			return get_argument_child(child, &str[1]);
		else
		{
			root->childs[c] = parg_alloc();
			child = root->childs[c];
			if (child != NULL)
				return get_argument_child(child, &str[1]);
			else
				return NULL;
		}
	}
	return root;
}

static int parse_argument(parg *root, int argc, char **argv, int n)
{
	unsigned int ret = n;
	unsigned int i = 0;
	char *arg = argv[n];
	parg *argument = root;
	char *value = NULL;
	if (arg[i] == '-')
	{
		int restriction = 1;
		++i;
		if (arg[i] == '-')
		{
			restriction = 2;
			++i;
			while (arg[i] != '=' && arg[i] != '\0')
			{
				if (argument == NULL)
					return n;
				argument = argument->childs[arg[i++]];
			}
			if (arg[i] == '=')
				++i;
			if (arg[i] != '\0')
				value = &arg[i];
		}
		else
		{
			while (arg[i] != '\0')
			{
				if (argument == NULL)
					return n;
				argument = argument->childs[arg[i++]];
			}
			if (n < argc - 1)
				value = argv[++n];
		}
		if (argument != NULL && argument != root && (argument->restriction == 0 || argument->restriction == restriction))
		{
			if (argument->vcallback != NULL)
			{
				if (value != NULL && value[0] != '\0')
				{
					argument->vcallback(value);
				}
			}
			else
			{
				argument->ecallback();
			}
		}
	}
	return n;
}

parg *parg_alloc()
{
	parg *argument = (parg *)malloc(sizeof(parg));
	if (argument != NULL)
	{
		argument->vcallback = NULL;
		argument->ecallback = NULL;
		for (unsigned int n = 0; n != CHAR_MAX; ++n)
			argument->childs[n] = NULL;
	}
	return argument;
}

void parg_free(parg *root)
{
	if (root != NULL)
	{
		for (unsigned int n = 0; n != CHAR_MAX; ++n)
		{
			if (root->childs[n] != NULL)
				parg_free(root->childs[n]);
		}
		free(root);
	}
}

void parg_add_vcallback(parg *root, const char *argument, void (*callback)(const char *), int restriction)
{
	if (root != NULL)
	{
		parg *new_argument = get_argument_child(root, argument);
		if (new_argument != NULL)
		{
			new_argument->restriction = restriction;
			new_argument->vcallback = callback;
			new_argument->ecallback = NULL;
		}
	}
}

void parg_add_ecallback(parg *root, const char *argument, void (*callback)())
{
	if (root != NULL)
	{
		parg *new_argument = get_argument_child(root, argument);
		if (new_argument != NULL)
		{
			new_argument->ecallback = callback;
			new_argument->vcallback = NULL;
		}
	}
}

void parg_parse(parg *root, int argc, char **argv)
{
	for (int n = 0; n != argc; ++n)
		parse_argument(root, argc, argv, n);
}