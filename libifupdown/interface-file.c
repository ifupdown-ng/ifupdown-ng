/*
 * libifupdown/interface-file.h
 * Purpose: /etc/network/interfaces parser
 *
 * Copyright (c) 2020 Ariadne Conill <ariadne@dereferenced.org>
 * Copyright (c) 2020 Maximilian Wilhelm <max@sdn.clinic>
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
#include <dirent.h>
#include <errno.h>
#include "libifupdown/libifupdown.h"

/* internally rewrite problematic ifupdown2 tokens to ifupdown-ng equivalents */
struct remap_token {
	const char *token;
	const char *alternative;
};

/* this list must be in alphabetical order for bsearch */
static const struct remap_token tokens[] = {
	{"bond-ad-sys-priority", "bond-ad-actor-sys-prio"}, /* ifupdown2 */
	{"bond-slaves", "bond-members"},		/* legacy ifupdown, ifupdown2 */
	{"client", "dhcp-client-id"},			/* legacy ifupdown */
	{"driver-message-level", "ethtool-msglvl"},	/* Debian ethtool integration */
	{"endpoint", "tunnel-remote"},			/* legacy ifupdown */
	{"ethernet-autoneg", "ethtool-ethernet-autoneg"},	/* Debian ethtool integration */
	{"ethernet-pause-autoneg", "ethtool-pause-autoneg"},	/* Debian ethtool integration */
	{"ethernet-pause-rx", "ethtool-pause-rx"},	/* Debian ethtool integration */
	{"ethernet-pause-tx", "ethtool-pause-tx"},	/* Debian ethtool integration */
	{"ethernet-port", "ethtool-ethernet-port"},	/* Debian ethtool integration */
	{"ethernet-wol", "ethtool-ethernet-wol"},	/* Debian ethtool integration */
	{"gro-offload", "ethtool-offload-gro"},		/* ifupdown2 */
	{"gso-offload", "ethtool-offload-gso"},		/* ifupdown2 */
	{"hardware-dma-ring-rx", "ethtool-dma-ring-rx"},		/* Debian ethtool integration */
	{"hardware-dma-ring-rx-jumbo", "ethtool-dma-ring-rx-jumbo"},	/* Debian ethtool integration */
	{"hardware-dma-ring-rx-mini", "ethtool-dma-ring-rx-mini"},	/* Debian ethtool integration */
	{"hardware-dma-ring-tx", "ethtool-dma-ring-tx"},		/* Debian ethtool integration */
	{"hardware-irq-coalesce-adaptive-rx", "ethtool-coalesce-adaptive-rx"},			/* Debian ethtool integration */
	{"hardware-irq-coalesce-adaptive-tx", "ethtool-coalesce-adaptive-tx"},			/* Debian ethtool integration */
	{"hardware-irq-coalesce-pkt-rate-high", "ethtool-coalesce-pkt-rate-high"},		/* Debian ethtool integration */
	{"hardware-irq-coalesce-pkt-rate-low", "ethtool-coalesce-pkt-rate-low"},		/* Debian ethtool integration */
	{"hardware-irq-coalesce-rx-frames", "ethtool-coalesce-rx-frames"},			/* Debian ethtool integration */
	{"hardware-irq-coalesce-rx-frames-high", "ethtool-coalesce-rx-frames-high"},		/* Debian ethtool integration */
	{"hardware-irq-coalesce-rx-frames-irq", "ethtool-coalesce-rx-frames-irq"},		/* Debian ethtool integration */
	{"hardware-irq-coalesce-rx-frames-low", "ethtool-coalesce-rx-frames-low"},		/* Debian ethtool integration */
	{"hardware-irq-coalesce-rx-usecs", "ethtool-coalesce-rx-usecs"},			/* Debian ethtool integration */
	{"hardware-irq-coalesce-rx-usecs-high", "ethtool-coalesce-rx-usecs-high"},		/* Debian ethtool integration */
	{"hardware-irq-coalesce-rx-usecs-irq", "ethtool-coalesce-rx-usecs-irq"},		/* Debian ethtool integration */
	{"hardware-irq-coalesce-rx-usecs-low", "ethtool-coalesce-rx-usecs-low"},		/* Debian ethtool integration */
	{"hardware-irq-coalesce-sample-interval", "ethtool-coalesce-sample-interval"},		/* Debian ethtool integration */
	{"hardware-irq-coalesce-stats-block-usecs", "ethtool-coalesce-stats-block-usecs"},	/* Debian ethtool integration */
	{"hardware-irq-coalesce-tx-frames", "ethtool-coalesce-tx-frames"},			/* Debian ethtool integration */
	{"hardware-irq-coalesce-tx-frames-high", "ethtool-coalesce-tx-frames-high"},		/* Debian ethtool integration */
	{"hardware-irq-coalesce-tx-frames-irq", "ethtool-coalesce-tx-frames-irq"},		/* Debian ethtool integration */
	{"hardware-irq-coalesce-tx-frames-low", "ethtool-coalesce-tx-frames-low"},		/* Debian ethtool integration */
	{"hardware-irq-coalesce-tx-usecs", "ethtool-coalesce-tx-usecs"},			/* Debian ethtool integration */
	{"hardware-irq-coalesce-tx-usecs-high", "ethtool-coalesce-tx-usecs-high"},		/* Debian ethtool integration */
	{"hardware-irq-coalesce-tx-usecs-irq", "ethtool-coalesce-tx-usecs-irq"},		/* Debian ethtool integration */
	{"hardware-irq-coalesce-tx-usecs-low", "ethtool-coalesce-tx-usecs-low"},		/* Debian ethtool integration */
	{"hostname", "dhcp-hostname"},			/* legacy ifupdown */
	{"leasetime", "dhcp-leastime"},			/* legacy ifupdown */
	{"link-autoneg", "ethtool-ethernet-autoneg"},	/* ifupdown2 */
	{"link-duplex", "ethtool-link-duplex"},		/* Debian ethtool integration */
	{"link-fec", "ethtool-link-fec"},		/* ifupdown2 */
	{"link-speed", "ethtool-link-speed"},		/* Debian ethtool integration */
	{"local", "tunnel-local"},			/* legacy ifupdown */
	{"lro-offload", "ethtool-offload-lro"},		/* ifupdown2 */
	{"mode", "tunnel-mode"},			/* legacy ifupdown */
	{"offload-gro", "ethtool-offload-gro"},		/* Debian ethtool integration */
	{"offload-gso", "ethtool-offload-gso"},		/* Debian ethtool integration */
	{"offload-lro", "ethtool-offload-lro"},		/* Debian ethtool integration */
	{"offload-rx", "ethtool-offload-rx"},		/* Debian ethtool integration */
	{"offload-sg", "ethtool-offload-sg"},		/* Debian ethtool integration */
	{"offload-tso", "ethtool-offload-tso"},		/* Debian ethtool integration */
	{"offload-tx", "ethtool-offload-tx"},		/* Debian ethtool integration */
	{"offload-ufo", "ethtool-offload-ufo"},		/* Debian ethtool integration */
	{"pointopoint", "point-to-point"},		/* legacy ifupdown, ifupdown2 */
	{"provider", "ppp-provider"},			/* legacy ifupdown, ifupdown2 */
	{"script", "dhcp-script"},			/* legacy ifupdown */
	{"rx-offload", "ethtool-offload-rx"},		/* ifupdown2 */
	{"tso-offload", "ethtool-offload-tso"},		/* ifupdown2 */
	{"ttl", "tunnel-ttl"},				/* legacy ifupdown */
	{"tunnel-endpoint", "tunnel-remote"},		/* ifupdown2 */
	{"tunnel-physdev", "tunnel-dev"},		/* ifupdown2 */
	{"tx-offload", "ethtool-offload-tx"},		/* ifupdown2 */
	{"ufo-offload", "ethtool-offload-ufo"},		/* ifupdown2 */
	{"vendor", "dhcp-vendor"},			/* legacy ifupdown */
	{"vrf", "vrf-member"},				/* ifupdown2 */
	{"vxlan-local-tunnelip", "vxlan-local-ip"},	/* ifupdown2 */
	{"vxlan-remoteip", "vxlan-remote-ip"},		/* ifupdown2 */
	{"vxlan-svcnodeip", "vxlan-remote-group"},	/* ifupdown2 */
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

static void
report_error(struct lif_interface_file_parse_state *state, const char *errfmt, ...)
{
	char errbuf[4096];

	va_list va;
	va_start(va, errfmt);
	vsnprintf(errbuf, sizeof errbuf, errfmt, va);
	va_end(va);

	fprintf(stderr, "%s:%zu: %s\n", state->cur_filename, state->cur_lineno, errbuf);
}

static bool
handle_address(struct lif_interface_file_parse_state *state, char *token, char *bufp)
{
	(void) token;

	char *addr = lif_next_token(&bufp);

	if (state->cur_iface == NULL)
	{
		report_error(state, "%s '%s' without interface", token, addr);
		/* Ignore this address, but don't fail hard */
		return true;
	}

	lif_interface_address_add(state->cur_iface, addr);

	return true;
}

static bool
handle_auto(struct lif_interface_file_parse_state *state, char *token, char *bufp)
{
	(void) token;

	char *ifname = lif_next_token(&bufp);
	if (!*ifname && state->cur_iface == NULL)
	{
		report_error(state, "auto without interface");
		return true;
	}
	else
	{
		state->cur_iface = lif_interface_collection_find(state->collection, ifname);
		if (state->cur_iface == NULL)
			return false;
	}

	if (!state->cur_iface->is_template)
		state->cur_iface->is_auto = true;

	if (state->cur_iface->is_auto)
		state->cur_iface->is_explicit = true;

	return true;
}

static bool
handle_gateway(struct lif_interface_file_parse_state *state, char *token, char *bufp)
{
	(void) token;

	char *addr = lif_next_token(&bufp);

	if (state->cur_iface == NULL)
	{
		report_error(state, "%s '%s' without interface", token, addr);
		/* Ignore this gateway, but don't fail hard */
		return true;
	}

	lif_interface_use_executor(state->cur_iface, "static");
	lif_dict_add(&state->cur_iface->vars, token, strdup(addr));

	return true;
}

static bool
handle_generic(struct lif_interface_file_parse_state *state, char *token, char *bufp)
{
	if (state->cur_iface == NULL)
		return true;

	token = maybe_remap_token(token);

	/* This smells like a bridge */
	if (strcmp(token, "bridge-ports") == 0)
		state->cur_iface->is_bridge = true;

	/* Skip any leading whitespaces in value for <token> */
	while (isspace (*bufp))
		bufp++;

	lif_dict_add(&state->cur_iface->vars, token, strdup(bufp));

	if (!lif_config.auto_executor_selection)
		return true;

	/* Check if token looks like <word1>-<word*> and assume <word1> is an addon */
	char *word_end = strchr(token, '-');
	if (word_end != NULL)
	{
		/* Copy word1 to not mangle *token */
		char *addon = strndup(token, word_end - token);
		lif_interface_use_executor(state->cur_iface, addon);
		free(addon);
	}

	return true;
}

static bool
handle_hostname(struct lif_interface_file_parse_state *state, char *token, char *bufp)
{
	char *hostname = lif_next_token(&bufp);

	if (state->cur_iface == NULL)
	{
		report_error(state, "%s '%s' without interface", token, hostname);
		/* Ignore this hostname, but don't fail hard */
		return true;
	}

	lif_dict_delete(&state->cur_iface->vars, token);
	lif_dict_add(&state->cur_iface->vars, token, strdup(hostname));

	return true;
}

static bool handle_inherit(struct lif_interface_file_parse_state *state, char *token, char *bufp);

static bool
handle_iface(struct lif_interface_file_parse_state *state, char *token, char *bufp)
{
	char *ifname = lif_next_token(&bufp);
	if (!*ifname)
	{
		report_error(state, "%s without any other tokens", token);
		/* This is broken but not fatal */
		return true;
	}

	state->cur_iface = lif_interface_collection_find(state->collection, ifname);
	if (state->cur_iface == NULL)
	{
		report_error(state, "could not upsert interface %s", ifname);
		return false;
	}

	/* mark the state->cur_iface as a template iface if `template` keyword
	 * is used.
	 */
	if (!strcmp(token, "template"))
	{
		state->cur_iface->is_auto = false;
		state->cur_iface->is_template = true;
	}

	/* in original ifupdown config, we can have "inet loopback"
	 * or "inet dhcp" or such to designate hints.  lets pick up
	 * those hints here.
	 */
	token = lif_next_token(&bufp);
	while (*token)
	{
		if (!strcmp(token, "dhcp"))
			lif_interface_use_executor(state->cur_iface, "dhcp");
		else if (!strcmp(token, "ppp"))
			lif_interface_use_executor(state->cur_iface, "ppp");
		else if (!strcmp(token, "inherits"))
		{
			if (!handle_inherit(state, token, bufp))
				return false;
		}

		token = lif_next_token(&bufp);
	}

	return true;
}

static bool
handle_inherit(struct lif_interface_file_parse_state *state, char *token, char *bufp)
{
	char *target = lif_next_token(&bufp);

	if (state->cur_iface == NULL)
	{
		report_error(state, "%s '%s' without interface", token, target);
		/* This is broken but not fatal */
		return true;
	}

	if (!*target)
	{
		report_error(state, "iface %s: unspecified inherit target", state->cur_iface->ifname);
		/* Mark this interface as errornous but carry on */
		state->cur_iface->has_config_error = true;
		return true;
	}

	struct lif_interface *parent = lif_interface_collection_find(state->collection, target);
	if (parent == NULL)
	{
		report_error(state, "iface %s: could not inherit from %s: not found",
		             state->cur_iface->ifname, target);
		/* Mark this interface as errornous but carry on */
		state->cur_iface->has_config_error = true;
		return true;

	}

	if (!lif_config.allow_any_iface_as_template && !parent->is_template)
	{
		report_error(state, "iface %s: could not inherit from %ss: inheritence from non-template interface not allowed",
		             state->cur_iface->ifname, target);
		/* Mark this interface as errornous but carry on */
		state->cur_iface->has_config_error = true;
		return true;
	}

	if (!lif_interface_collection_inherit(state->cur_iface, parent))
	{
		report_error(state, "iface %s: could not inherit from %s", state->cur_iface->ifname, target);
		/* Mark this interface as errornous but carry on */
		state->cur_iface->has_config_error = true;
		return true;
	}

	return true;
}

static bool
handle_source(struct lif_interface_file_parse_state *state, char *token, char *bufp)
{
	(void) token;

	char *source_filename = lif_next_token(&bufp);
	if (!*source_filename)
	{
		report_error(state, "missing filename to source");
		/* Broken but not fatal */
		return true;
	}

	return lif_interface_file_parse(state, source_filename);
}

static bool
handle_source_directory(struct lif_interface_file_parse_state *state, char *token, char *bufp)
{
	(void) token;

	char *source_directory = lif_next_token(&bufp);
	if (!*source_directory)
	{
		report_error(state, "missing directory to source");
		/* Broken but not fatal */
		return true;
	}

	DIR *source_dir = opendir(source_directory);
	if (source_dir == NULL)
	{
		report_error(state, "while opening directory %s: %s", source_directory, strerror(errno));
		/* Broken but not fatal */
		return true;
	}

	struct dirent *dirent_p;
	for (dirent_p = readdir(source_dir); dirent_p != NULL; dirent_p = readdir(source_dir))
	{
		if (dirent_p->d_type != DT_REG)
			continue;

		char pathbuf[4096];
		snprintf(pathbuf, sizeof pathbuf, "%s/%s", source_directory, dirent_p->d_name);

		if (!lif_interface_file_parse(state, pathbuf))
		{
			closedir(source_dir);
			return false;
		}
	}

	closedir(source_dir);
	return true;
}

static bool
handle_use(struct lif_interface_file_parse_state *state, char *token, char *bufp)
{
	char *executor = lif_next_token(&bufp);

	if (state->cur_iface == NULL)
	{
		report_error(state, "%s '%s' without interface", token, executor);
		/* Broken but not fatal */
		return true;
	}

	lif_interface_use_executor(state->cur_iface, executor);
	return true;
}

/* map keywords to parser functions */
struct parser_keyword {
	const char *token;
	bool (*handle)(struct lif_interface_file_parse_state *state, char *token, char *bufp);
};

static const struct parser_keyword keywords[] = {
	{"address", handle_address},
	{"auto", handle_auto},
	{"gateway", handle_gateway},
	{"hostname", handle_hostname},
	{"iface", handle_iface},
	{"inherit", handle_inherit},
	{"interface", handle_iface},
	{"source", handle_source},
	{"source-directory", handle_source_directory},
	{"template", handle_iface},
	{"use", handle_use},
};

static int
keyword_cmp(const void *a, const void *b)
{
	const char *key = a;
	const struct parser_keyword *token = b;

	return strcmp(key, token->token);
}

bool
lif_interface_file_parse(struct lif_interface_file_parse_state *state, const char *filename)
{
	struct lif_dict_entry *entry = lif_dict_find(&state->loaded, filename);
	if (entry != NULL)
	{
		report_error(state, "skipping already included file %s", filename);
		return true;
	}

	FILE *f = fopen(filename, "r");
	if (f == NULL)
		return false;

	const char *old_filename = state->cur_filename;
	state->cur_filename = filename;

	size_t old_lineno = state->cur_lineno;
	state->cur_lineno = 0;

	lif_dict_add(&state->loaded, filename, NULL);

	char linebuf[4096];
	while (lif_fgetline(linebuf, sizeof linebuf, f) != NULL)
	{
		state->cur_lineno++;

		char *bufp = linebuf;
		char *token = lif_next_token(&bufp);

		if (!*token || !isalpha(*token))
			continue;

		const struct parser_keyword *parserkw =
			bsearch(token, keywords, ARRAY_SIZE(keywords), sizeof(*keywords), keyword_cmp);

		if (parserkw != NULL)
		{
			if (!parserkw->handle(state, token, bufp))
				goto parse_error;
		}
		else if (!handle_generic(state, token, bufp))
			goto parse_error;
	}

	fclose(f);
	state->cur_filename = old_filename;
	state->cur_lineno = old_lineno;
	return true;

parse_error:
	fclose(f);
	state->cur_filename = old_filename;
	state->cur_lineno = old_lineno;
	return false;
}
