ifcrc32sum(5)

# NAME

*ifcrc32sum* - CRC32 checksum calculatation for an interface to generate VRRP interfaces names

# SYNOPSIS

ifcrc32sum [<_options_>...] <interface>

# DESCRIPTION

*ifcrc32sum* is used to generate a 32-bit CRC32 value of an interface name and to be used by VRRP
executor to generate MACVLAN subinterfaces. This tool is based on cksum from Linux coreutils package
and it has a compatible output for better interoperability and portability.

# OPTIONS

*-X, --hex*
	Display HEX value of the CRC32 directly. Used by VRRP executor to not used *printf*.

*-h, --help*
	Display supported options to ifquery.

*-V, --version*
	Print the ifupdown-ng version and exit.

# SEE ALSO

*ifupdown-ng.conf*(5)
*ifup*(8)
*ifquery*(8)
*interfaces*(5)

# AUTHORS

Adrian Ban @EasyNetDev <devel@easynet.dev>
