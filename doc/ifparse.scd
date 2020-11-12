ifparse(8)

# NAME

ifparse - redisplay interface configuration in different formats

# SYNOPSIS

*ifparse* [<_options_>...] <_interfaces_...>

*ifparse* -A|--all

# DESCRIPTION

*ifparse* is used to extract information from the interface configuration
file.  It is intended to be used to translate the interface configuration
stanzas between different formats.

# OPTIONS

*-a, --auto*
	Only match interfaces that are marked as _auto_.

*-h, --help*
	Display supported options to ifquery.

*-i, --interfaces* _FILE_
	Use _FILE_ as the config database.

*-I, --include* _PATTERN_
	Include _PATTERN_ when matching against the config or state
	database.

*-U, --allow-undefined*
	Create virtual interfaces for any interfaces not explicitly
	defined in the configuration file.  This is primarily useful
	for property queries.

*-S, --state-file* _FILE_
	Use _FILE_ as the state database.

*-V, --version*
	Print the ifupdown-ng version and exit.

*-X, --exclude* _PATTERN_
	Exclude _PATTERN_ when matching against the config or state
	database.

*-Y, --yaml-raw*
	Output configuration data in the yaml-raw format instead of
	the traditional stanza-based format.  This format is primarily
	intended for other tools outside ifupdown-ng itself to use.

# SEE ALSO

*ifup*(8)++
*ifdown*(8)++
*ifquery*(8)++
*interfaces*(5)

# AUTHORS

Ariadne Conill <ariadne@dereferenced.org>
