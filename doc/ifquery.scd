ifquery(8)

# NAME

ifquery - query interface configuration and state

# SYNOPSIS

*ifquery* [<_options_>...] <_interfaces_...>

*ifquery* -L|--list

*ifquery* -s|--state

# DESCRIPTION

*ifquery* is used to extract information from the interface configuration
file.  It can also be used to convert from old versions of the interface
configuration file to the current format.

# OPTIONS

*-a, --auto*
	Only match interfaces that are marked as _auto_.

*-h, --help*
	Display supported options to ifquery.

*-i, --interfaces* _FILE_
	Use _FILE_ as the config database.

*-p, --property* _PROPERTY_
	Print the values of matching properties for an interface.

*-r, --running*
	Print the interface names that are marked as running in
	the state database.

*-s, --state*
	Query the state database instead of the config database.

*-D, --dot*
	Generate a dependency graph that can be used with GraphViz
	*dot*(1).  Used with *--list*.

*-I, --include* _PATTERN_
	Include _PATTERN_ when matching against the config or state
	database.

*-L, --list*
	List interfaces which exist in the configuration database.

*-P, --pretty-print*
	When listing interfaces, print their configuration in a format
	that is compatible with *interfaces*(5) files.

*-U, --allow-undefined*
	Create virtual interfaces for any interfaces not explicitly
	defined in the configuration file.  This is primarily useful
	for property queries.

*-S, --state-file* _FILE_
	Use _FILE_ as the state database.

*-T, --timeout* _TIMEOUT_
	Wait up to _TIMEOUT_ seconds for executors to complete before
	raising an error.

*-V, --version*
	Print the ifupdown-ng version and exit.

*-X, --exclude* _PATTERN_
	Exclude _PATTERN_ when matching against the config or state
	database.

# SEE ALSO

*ifup*(8)++
*ifdown*(8)++
*interfaces*(5)

# AUTHORS

Ariadne Conill <ariadne@dereferenced.org>
