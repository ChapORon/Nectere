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
		int restriction = PARG_SIMPLE_DASH_RESTRICTION;
		++i;
		if (arg[i] == '-')
		{
			restriction = PARG_DOUBLE_DASH_RESTRICTION;
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
			if (n < argc - 1 && argument->value_restriction != PARG_NEED_NO_VALUE)
			{
				if (argv[n + 1][0] != '-' || argument->value_restriction == PARG_NEED_VALUE)
					value = argv[++n];
			}
		}
		if ((value == NULL && argument->value_restriction == PARG_NEED_VALUE) ||
			(value != NULL && argument->value_restriction == PARG_NEED_NO_VALUE))
			return n;
		if (argument != NULL && argument->callback != NULL && argument != root && (argument->restriction == 0 || argument->restriction == restriction))
		{
			if (value == NULL || value[0] != '\0')
			{
				argument->callback(argument->callback_data,argument->name, value, argument->restriction);
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
		argument->restriction = PARG_NO_RESTRICTION;
		argument->value_restriction = PARG_OPTIONNAL_VALUE;
		argument->name = NULL;
		argument->callback = NULL;
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

void parg_add_callback(parg *root, const char *argument, void (*callback)(void *, const char *, const char *, int), void *data, int restriction, int value_restriction)
{
	if (root != NULL)
	{
		parg *new_argument = get_argument_child(root, argument);
		if (new_argument != NULL)
		{
			new_argument->restriction = restriction;
			new_argument->value_restriction = value_restriction;
			new_argument->name = argument;
			new_argument->callback_data = data;
			new_argument->callback = callback;
		}
	}
}

void parg_parse(parg *root, int argc, char **argv)
{
	for (int n = 0; n != argc; ++n)
		parse_argument(root, argc, argv, n);
}