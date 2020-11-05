/*
 * libifupdown/config-file.h
 * Purpose: config file loading
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

#ifndef LIBIFUPDOWN__CONFIG_FILE_H
#define LIBIFUPDOWN__CONFIG_FILE_H

#include <stdbool.h>

struct lif_config_file {
	bool allow_addon_scripts;
	bool allow_any_iface_as_template;
	bool auto_executor_selection;
	bool compat_create_interfaces;
	bool compat_ifupdown2_bridge_ports_inherit_vlans;
	bool implicit_template_conversion;
	bool use_hostname_for_dhcp;
};

extern struct lif_config_file lif_config;

extern bool lif_config_load(const char *filename);

#endif
