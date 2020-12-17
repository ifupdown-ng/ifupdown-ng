interfaces-mpls(5)

# NAME

*interfaces-mpls* - MPLS vocabulary for the interfaces(5) file format

# DESCRIPTION

Linux allows has support for MultiProtocol Label Switching (MPLS) for a while
now.  The following options allow for this configuration.

# MPLS-RELATED OPTIONS

The MPLS executor will only modify the sysctl configuration if these options
are provided, otherwise other mechanisms such as /etc/sysctl.conf may be used.
If MPLS is enabled on (at least) one interface the executor will load the
_mpls_iptunnel_ kernel module.

Be aware that you have to set the _platform_labels_ sysctl to make MPLS work.
See https://www.kernel.org/doc/Documentation/networking/mpls-sysctl.rst for
more details on the MPLS related knobs in the Linux kernel.


*mpls-enable* _yes|no_
	Control whether packets can be input on this interface.  If disabled,
	packets carrying an MPLS label will be discarded without further
	processing.

# EXAMPLES

```
iface eth0
	address 2001:db8:08:15::42/64
	#
	mpls-enable yes
```

# AUTHORS

Maximilian Wilhelm <max@sdn.clinic>
