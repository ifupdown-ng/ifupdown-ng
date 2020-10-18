ifdown(8)

# NAME

ifdown - take interfaces down

# SYNOPSIS

ifdown [<_options_>...] <_interfaces_>

# DESCRIPTION

*ifdown* is used to deconfigure interfaces according to how they are
configured in the configuration database.

# OPTIONS

*-a, --auto*
	Only match interfaces that are marked as _auto_.

*-f, --force*
	Force deconfiguration of the interface.
	This option exists for compatibility with other implementations.

*-h, --help*
	Display supported options to ifquery.

*-i, --interfaces* _FILE_
	Use _FILE_ as the config database.

*-n, --no-act*
	Show what commands would be run instead of actually running
	them.  Useful for testing configuration changes.

*-v, --verbose*
	Show what commands are being run as they are executed.

*-E, --executor-path* _PATH_
	Look for executors in the given _PATH_.

*-I, --include* _PATTERN_
	Include _PATTERN_ when matching against the config or state
	database.

*-S, --state-file* _FILE_
	Use _FILE_ as the state database.

*-V, --version*
	Print the ifupdown-ng version and exit.

*-X, --exclude* _PATTERN_
	Exclude _PATTERN_ when matching against the config or state
	database.

# SEE ALSO

*ifupdown-ng.conf*(5)
*ifup*(8)
*ifquery*(8)
*interfaces*(5)

# AUTHORS

Ariadne Conill <ariadne@dereferenced.org>
