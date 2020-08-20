/*
 * libifupdown/interface-file.h
 * Purpose: /etc/network/interfaces parser
 *
 * Copyright (c) 2020 Ariadne Conill <ariadne@dereferenced.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * This software is provided 'as is' and without any warranty, express or
 * implied.  In no event shall the authors be liable for any damages arising
 * from the use of this software.
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "libifupdown/libifupdown.h"

/* internally rewrite problematic ifupdown2 tokens to ifupdown-ng equivalents */
struct remap_token {
	const char *token;
	const char *alternative;
};

/* this list must be in alphabetical order for bsearch */
static const struct remap_token tokens[] = {
	{"endpoint", "tunnel-remote"},			/* legacy ifupdown */
	{"local", "tunnel-local"},			/* legacy ifupdown */
	{"mode", "tunnel-mode"},			/* legacy ifupdown */
	{"provider", "ppp-provider"},			/* legacy ifupdown, ifupdown2 */
	{"ttl", "tunnel-ttl"},				/* legacy ifupdown */
	{"tunnel-endpoint", "tunnel-remote"},		/* ifupdown2 */
	{"tunnel-physdev", "tunnel-dev"},		/* ifupdown2 */
	{"vrf", "vrf-member"},				/* ifupdown2 */
};

static int
token_cmp(const void *a, const void *b)
{
	const char *key = a;
	const struct remap_token *token = b;

	return strcmp(key, token->token);
}

static char *
maybe_remap_token(const char *token)
{
	const struct remap_token *tok = NULL;
	static char tokbuf[4096];

	tok = bsearch(token, tokens, ARRAY_SIZE(tokens), sizeof(*tokens), token_cmp);
	strlcpy(tokbuf, tok != NULL ? tok->alternative : token, sizeof tokbuf);

	return tokbuf;
}

/* map keywords to parser functions */
struct parser_keyword {
	const char *token;
	bool (*handle)(struct lif_dict *collection, struct lif_interface *cur_iface, const char *filename, size_t lineno, char *token, char *bufp);
};

static const struct parser_keyword keywords[] = {
};

static int
keyword_cmp(const void *a, const void *b)
{
	const char *key = a;
	const struct parser_keyword *token = b;

	return strcmp(key, token->token);
}

static void
report_error(const char *filename, size_t lineno, const char *errfmt, ...)
{
	char errbuf[4096];

	va_list va;
	va_start(va, errfmt);
	vsnprintf(errbuf, sizeof errbuf, errfmt, va);
	va_end(va);

	fprintf(stderr, "%s:%zu: %s\n", filename, lineno, errbuf);
}

bool
lif_interface_file_parse(struct lif_dict *collection, const char *filename)
{
	lif_interface_collection_init(collection);
	struct lif_interface *cur_iface = NULL;

	FILE *f = fopen(filename, "r");
	if (f == NULL)
		return false;

	char linebuf[4096];
	size_t lineno = 0;
	while (lif_fgetline(linebuf, sizeof linebuf, f) != NULL)
	{
		lineno++;

		char *bufp = linebuf;
		char *token = lif_next_token(&bufp);

		if (!*token || !isalpha(*token))
			continue;

		const struct parser_keyword *parserkw =
			bsearch(token, keywords, ARRAY_SIZE(keywords), sizeof(*keywords), keyword_cmp);

		if (parserkw != NULL)
		{
			if (!parserkw->handle(collection, cur_iface, filename, lineno, token, bufp))
				goto parse_error;
		}
		else if (!strcmp(token, "source"))
		{
			char *source_filename = lif_next_token(&bufp);
			if (!*source_filename)
				goto parse_error;

			if (!strcmp(filename, source_filename))
			{
				report_error(filename, lineno, "attempt to source %s would create infinite loop",
					     source_filename);
				goto parse_error;
			}

			lif_interface_file_parse(collection, source_filename);
		}
		else if (!strcmp(token, "auto"))
		{
			char *ifname = lif_next_token(&bufp);
			if (!*ifname && cur_iface == NULL)
				goto parse_error;
			else
			{
				cur_iface = lif_interface_collection_find(collection, ifname);
				if (cur_iface == NULL)
					goto parse_error;
			}

			cur_iface->is_auto = true;
		}
		else if (!strcmp(token, "iface"))
		{
			char *ifname = lif_next_token(&bufp);
			if (!*ifname)
				goto parse_error;

			cur_iface = lif_interface_collection_find(collection, ifname);
			if (cur_iface == NULL)
				goto parse_error;

			/* in original ifupdown config, we can have "inet loopback"
			 * or "inet dhcp" or such to designate hints.  lets pick up
			 * those hints here.
			 */
			char *token = lif_next_token(&bufp);
			while (*token)
			{
				if (!strcmp(token, "dhcp"))
					lif_interface_use_executor(cur_iface, "dhcp");
				else if (!strcmp(token, "ppp"))
					lif_interface_use_executor(cur_iface, "ppp");
				else if (!strcmp(token, "inherits"))
				{
					token = lif_next_token(&bufp);

					if (!*token)
					{
						report_error(filename, lineno, "inherits without interface");
						goto parse_error;
					}

					if (!lif_interface_collection_inherit(cur_iface, collection, token))
					{
						report_error(filename, lineno, "could not inherit %s", token);
						goto parse_error;
					}
				}

				token = lif_next_token(&bufp);
			}
		}
		else if (!strcmp(token, "use"))
		{
			char *executor = lif_next_token(&bufp);

			if (cur_iface == NULL)
			{
				report_error(filename, lineno, "use '%s' without interface", executor);
				goto parse_error;
			}

			/* pass requires as compatibility env vars to appropriate executors (bridge, bond) */
			if (!strcmp(executor, "bridge"))
				cur_iface->is_bridge = true;
			else if (!strcmp(executor, "bond"))
				cur_iface->is_bond = true;

			lif_interface_use_executor(cur_iface, executor);
		}
		else if (!strcmp(token, "inherit"))
		{
			token = lif_next_token(&bufp);

			if (!*token)
			{
				report_error(filename, lineno, "inherits without interface");
				goto parse_error;
			}

			if (!lif_interface_collection_inherit(cur_iface, collection, token))
			{
				report_error(filename, lineno, "could not inherit %s", token);
				goto parse_error;
			}
		}
		else if (!strcmp(token, "address"))
		{
			char *addr = lif_next_token(&bufp);

			if (cur_iface == NULL)
			{
				report_error(filename, lineno, "%s: address '%s' without interface", filename, addr);
				goto parse_error;
			}

			lif_interface_address_add(cur_iface, addr);
		}
		else if (!strcmp(token, "gateway"))
		{
			char *addr = lif_next_token(&bufp);

			if (cur_iface == NULL)
			{
				report_error(filename, lineno, "%s: gateway '%s' without interface", filename, addr);
				goto parse_error;
			}

			lif_interface_use_executor(cur_iface, "static");
			lif_dict_add(&cur_iface->vars, token, strdup(addr));
		}
		else if (cur_iface != NULL)
		{
			token = maybe_remap_token(token);

			lif_dict_add(&cur_iface->vars, token, strdup(bufp));

			/* Check if token looks like <word1>-<word*> and assume <word1> is an addon */
			char *word_end = strchr(token, '-');
			if (word_end != NULL)
			{
				/* Copy word1 to not mangle *token */
				char *addon = strndup(token, word_end - token);
				lif_interface_use_executor(cur_iface, addon);
				free(addon);

				/* pass requires as compatibility env vars to appropriate executors (bridge, bond) */
				if (!strcmp(addon, "bridge"))
					cur_iface->is_bridge = true;
				else if (!strcmp(addon, "bond"))
					cur_iface->is_bond = true;
			}
		}
	}

	fclose(f);
	return true;

parse_error:
	fclose(f);
	return false;
}
