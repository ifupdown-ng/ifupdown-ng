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

#include <string.h>

#include "libifupdown/environment.h"
#include "libifupdown/execute.h"
#include "libifupdown/interface.h"
#include "libifupdown/lifecycle.h"
#include "libifupdown/state.h"

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

		if (strcmp(entry->key, "address"))
			continue;

		struct lif_address *addr = entry->data;
		char addrbuf[4096];

		if (!lif_address_unparse(addr, addrbuf, sizeof addrbuf, true))
			return false;

		if (!lif_execute_fmt(opts, NULL, "/sbin/ip addr add %s dev %s", addrbuf, lifname))
			return false;
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

		if (strcmp(entry->key, "address"))
			continue;

		struct lif_address *addr = entry->data;
		char addrbuf[4096];

		if (!lif_address_unparse(addr, addrbuf, sizeof addrbuf, true))
			return false;

		if (!lif_execute_fmt(opts, NULL, "/sbin/ip addr del %s dev %s", addrbuf, lifname))
			return false;
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

	lif_environment_push(&envp, "IFACE", iface->ifname);
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

	handle_commands_for_phase(opts, envp, iface, lifname, phase);

	/* we should do error handling here, but ifupdown1 doesn't */
	lif_execute_fmt(opts, envp, "run-parts /etc/network/if-%s.d", phase);

	lif_environment_free(&envp);
	return true;

on_error:
	lif_environment_free(&envp);
	return false;
}

bool
lif_lifecycle_run(const struct lif_execute_opts *opts, struct lif_interface *iface, struct lif_dict *state, const char *lifname, bool up)
{
	if (lifname == NULL)
		lifname = iface->ifname;

	if (up)
	{
		/* XXX: we should try to recover (take the iface down) if bringing it up fails.
		 * but, right now neither debian ifupdown or busybox ifupdown do any recovery,
		 * so we wont right now.
		 */
		if (!lif_lifecycle_run_phase(opts, iface, "pre-up", lifname, up))
			return false;

		if (!lif_lifecycle_run_phase(opts, iface, "up", lifname, up))
			return false;

		lif_state_upsert(state, lifname, iface);
	}
	else
	{
		if (!lif_lifecycle_run_phase(opts, iface, "down", lifname, up))
			return false;

		if (!lif_lifecycle_run_phase(opts, iface, "post-down", lifname, up))
			return false;

		lif_state_delete(state, lifname);
	}

	return true;
}
