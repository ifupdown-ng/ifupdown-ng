/*
 * libifupdown/lifecycle.c
 * Purpose: management of interface lifecycle (bring up, takedown, reload)
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
#include <string.h>

#include "libifupdown/environment.h"
#include "libifupdown/execute.h"
#include "libifupdown/interface.h"
#include "libifupdown/lifecycle.h"
#include "libifupdown/state.h"
#include "libifupdown/tokenize.h"

static bool
handle_commands_for_phase(const struct lif_execute_opts *opts, char *const envp[], struct lif_interface *iface, const char *lifname, const char *phase)
{
	struct lif_node *iter;

	(void) lifname;

	LIF_DICT_FOREACH(iter, &iface->vars)
	{
		struct lif_dict_entry *entry = iter->data;

		if (strcmp(entry->key, phase))
			continue;

		const char *cmd = entry->data;
		if (!lif_execute_fmt(opts, envp, "%s", cmd))
			return false;
	}

	return true;
}

static bool
handle_executors_for_phase(const struct lif_execute_opts *opts, char *const envp[], struct lif_interface *iface)
{
	struct lif_node *iter;

	LIF_DICT_FOREACH(iter, &iface->vars)
	{
		struct lif_dict_entry *entry = iter->data;

		if (strcmp(entry->key, "use"))
			continue;

		const char *cmd = entry->data;
		if (!lif_maybe_run_executor(opts, envp, cmd))
			return false;
	}

	return true;
}

static inline size_t
count_set_bits(const char *netmask)
{
	/* netmask set to CIDR length */
	if (strchr(netmask, '.') == NULL)
		return strtol(netmask, NULL, 10);

	size_t r = 0;
	struct in_addr in;

	if (inet_pton(AF_INET, netmask, &in) == 0)
		return r;

	/* take the IP, put it in host endian order, and
	 * flip it so that all the set bits are set to the right.
	 * then we can simply count down from 32 and right-shift
	 * until the bit field is all zero.
	 */
	unsigned int bits = htonl(in.s_addr);
	for (bits = ~bits, r = 32; bits; bits >>= 1, r--)
		;

	return r;
}

static bool
handle_address(const struct lif_execute_opts *opts, struct lif_address *addr, const char *cmd, const char *lifname, struct lif_interface *iface)
{
	char addrbuf[4096];
	size_t orig_netmask = addr->netmask;

	if (!addr->netmask)
	{
		/* if fallback netmask is not set, default to 255.255.255.0 */
		addr->netmask = 24;

		struct lif_dict_entry *entry = lif_dict_find(&iface->vars, "netmask");
		if (entry != NULL)
			addr->netmask = count_set_bits(entry->data);
	}

	if (!lif_address_unparse(addr, addrbuf, sizeof addrbuf, true))
		return false;

	/* reset the netmask */
	addr->netmask = orig_netmask;

	return lif_execute_fmt(opts, NULL, "/sbin/ip -%d addr %s %s dev %s",
			       addr->domain == AF_INET ? 4 : 6, cmd, addrbuf, lifname);
}

static bool
handle_gateway(const struct lif_execute_opts *opts, const char *gateway, const char *cmd)
{
	int ipver = strchr(gateway, ':') ? 6 : 4;

	return lif_execute_fmt(opts, NULL, "/sbin/ip -%d route %s default via %s",
			       ipver, cmd, gateway);
}

static bool
handle_pre_up(const struct lif_execute_opts *opts, struct lif_interface *iface, const char *lifname)
{
	(void) opts;
	(void) iface;
	(void) lifname;

	return true;
}

static bool
handle_up(const struct lif_execute_opts *opts, struct lif_interface *iface, const char *lifname)
{
	struct lif_node *iter;

	if (!lif_execute_fmt(opts, NULL, "/sbin/ip link set up dev %s", lifname))
		return false;

	if (iface->is_loopback)
		return true;

	LIF_DICT_FOREACH(iter, &iface->vars)
	{
		struct lif_dict_entry *entry = iter->data;

		if (!strcmp(entry->key, "address"))
		{
			struct lif_address *addr = entry->data;

			if (!handle_address(opts, addr, "add", lifname, iface))
				return false;
		}
		else if (!strcmp(entry->key, "gateway"))
		{
			if (!handle_gateway(opts, entry->data, "add"))
				return false;
		}
	}

	return true;
}

static bool
handle_down(const struct lif_execute_opts *opts, struct lif_interface *iface, const char *lifname)
{
	struct lif_node *iter;

	if (iface->is_loopback)
		goto skip_addresses;

	LIF_DICT_FOREACH(iter, &iface->vars)
	{
		struct lif_dict_entry *entry = iter->data;

		if (!strcmp(entry->key, "address"))
		{
			struct lif_address *addr = entry->data;

			if (!handle_address(opts, addr, "del", lifname, iface))
				return false;
		}
		else if (!strcmp(entry->key, "gateway"))
		{
			if (!handle_gateway(opts, entry->data, "del"))
				return false;
		}
	}

skip_addresses:
	if (!lif_execute_fmt(opts, NULL, "/sbin/ip link set down dev %s", lifname))
		return false;

	return true;
}

static bool
handle_post_down(const struct lif_execute_opts *opts, struct lif_interface *iface, const char *lifname)
{
	(void) opts;
	(void) iface;
	(void) lifname;

	return true;
}

bool
lif_lifecycle_run_phase(const struct lif_execute_opts *opts, struct lif_interface *iface, const char *phase, const char *lifname, bool up)
{
	char **envp = NULL;

	lif_environment_push(&envp, "IFACE", lifname);
	lif_environment_push(&envp, "PHASE", phase);

	/* try to provide $METHOD for ifupdown1 scripts if we can */
	if (iface->is_loopback)
		lif_environment_push(&envp, "METHOD", "loopback");
	else if (iface->is_dhcp)
		lif_environment_push(&envp, "METHOD", "dhcp");

	/* same for $MODE */
	if (up)
		lif_environment_push(&envp, "MODE", "start");
	else
		lif_environment_push(&envp, "MODE", "stop");

	if (opts->verbose)
		lif_environment_push(&envp, "VERBOSE", "1");

	if (opts->interfaces_file)
		lif_environment_push(&envp, "INTERFACES_FILE", opts->interfaces_file);

	struct lif_node *iter;
	bool did_address = false, did_gateway = false;

	LIF_DICT_FOREACH(iter, &iface->vars)
	{
		struct lif_dict_entry *entry = iter->data;

		if (!strcmp(entry->key, "address"))
		{
			if (did_address)
				continue;

			struct lif_address *addr = entry->data;
			char addrbuf[4096];

			if (!lif_address_unparse(addr, addrbuf, sizeof addrbuf, true))
				continue;

			lif_environment_push(&envp, "IF_ADDRESS", addrbuf);
			did_address = true;

			continue;
		}
		else if (!strcmp(entry->key, "gateway"))
		{
			if (did_gateway)
				continue;

			did_gateway = true;
		}
		else if (!strcmp(entry->key, "requires"))
		{
			if (iface->is_bridge)
				lif_environment_push(&envp, "IF_BRIDGE_PORTS", (const char *) entry->data);

			if (iface->is_bond)
				lif_environment_push(&envp, "IF_BOND_SLAVES", (const char *) entry->data);
		}

		char envkey[4096] = "IF_";
		strlcat(envkey, entry->key, sizeof envkey);
		char *ep = envkey + 2;

		while (*ep++)
		{
			*ep = toupper(*ep);

			if (*ep == '-')
				*ep = '_';
		}

		lif_environment_push(&envp, envkey, (const char *) entry->data);
	}

	if (!strcmp(phase, "pre-up"))
	{
		if (!handle_pre_up(opts, iface, lifname))
			goto on_error;
	}
	else if (!strcmp(phase, "up"))
	{
		if (!handle_up(opts, iface, lifname))
			goto on_error;
	}
	else if (!strcmp(phase, "down"))
	{
		if (!handle_down(opts, iface, lifname))
			goto on_error;
	}
	else if (!strcmp(phase, "post-down"))
	{
		if (!handle_post_down(opts, iface, lifname))
			goto on_error;
	}

	handle_executors_for_phase(opts, envp, iface);
	handle_commands_for_phase(opts, envp, iface, lifname, phase);

	/* we should do error handling here, but ifupdown1 doesn't */
	lif_execute_fmt(opts, envp, "/bin/run-parts /etc/network/if-%s.d", phase);

	lif_environment_free(&envp);
	return true;

on_error:
	lif_environment_free(&envp);
	return false;
}

static bool
handle_dependents(const struct lif_execute_opts *opts, struct lif_interface *parent, struct lif_dict *collection, struct lif_dict *state, bool up)
{
	struct lif_dict_entry *requires = lif_dict_find(&parent->vars, "requires");

	/* no dependents, nothing to worry about */
	if (requires == NULL)
		return true;

	char require_ifs[4096] = {};
	strlcpy(require_ifs, requires->data, sizeof require_ifs);
	char *bufp = require_ifs;

	for (char *tokenp = lif_next_token(&bufp); *tokenp; tokenp = lif_next_token(&bufp))
	{
		struct lif_interface *iface = lif_interface_collection_find(collection, tokenp);

		/* already up or down, skip */
		if (up == iface->is_up)
			continue;

		if (opts->verbose)
			fprintf(stderr, "ifupdown: changing state of dependent interface %s (of %s) to %s\n",
				iface->ifname, parent->ifname, up ? "up" : "down");

		if (!lif_lifecycle_run(opts, iface, collection, state, iface->ifname, up))
			return false;
	}

	return true;
}

bool
lif_lifecycle_run(const struct lif_execute_opts *opts, struct lif_interface *iface, struct lif_dict *collection, struct lif_dict *state, const char *lifname, bool up)
{
	if (lifname == NULL)
		lifname = iface->ifname;

	if (up)
	{
		/* when going up, dependents go up first. */
		if (!handle_dependents(opts, iface, collection, state, up))
			return false;

		/* XXX: we should try to recover (take the iface down) if bringing it up fails.
		 * but, right now neither debian ifupdown or busybox ifupdown do any recovery,
		 * so we wont right now.
		 */
		if (!lif_lifecycle_run_phase(opts, iface, "pre-up", lifname, up))
			return false;

		if (!lif_lifecycle_run_phase(opts, iface, "up", lifname, up))
			return false;

		if (!lif_lifecycle_run_phase(opts, iface, "post-up", lifname, up))
			return false;

		lif_state_upsert(state, lifname, iface);

		iface->is_up = true;
	}
	else
	{
		if (!lif_lifecycle_run_phase(opts, iface, "pre-down", lifname, up))
			return false;

		if (!lif_lifecycle_run_phase(opts, iface, "down", lifname, up))
			return false;

		if (!lif_lifecycle_run_phase(opts, iface, "post-down", lifname, up))
			return false;

		/* when going up, dependents go down last. */
		if (!handle_dependents(opts, iface, collection, state, up))
			return false;

		lif_state_delete(state, lifname);

		iface->is_up = false;
	}

	return true;
}
