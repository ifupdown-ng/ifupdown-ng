ifctrstat(8)

# NAME

ifctrstat - display interface statistics

# SYNOPSIS

*ifctrstat* [<_options_>...] <_interface_> <_counter_>

*ifctrstat* [<_options_>...] -L|--list

# DESCRIPTION

*ifctrstat* is used to query interface statistic counters in
a kernel-agnostic way.  This is useful for heterogenous
environments where multiple kernels may be in use.

# OPTIONS

*-h, --help*
	Display supported options to ifctrstat.

*-n, --no-label*
	Display the requested counter without its label.

*-L, --list*
	List available counters on this system.

*-V, --version*
	Print the ifupdown-ng version and exit.

# SEE ALSO

*ifquery*(8)

# AUTHORS

A. Wilcox <awilfox@adelielinux.org>
