interfaces-forward(5)

# NAME

*interfaces-forward* - forwarding vocabulary for the interfaces(5) file format

# DESCRIPTION

Linux allows for configuration of IP packet forwarding behavior on a protocol
and interface basis.  The following options allow for this configuration.

# FORWARDING-RELATED OPTIONS

The forward executor will only modify the sysctl configuration if these options
are provided, otherwise other mechanisms such as /etc/sysctl.conf may be used.

*forward-ipv4* _yes|no_
	Whether the interface should forward unicast IPv4 packets.

*forward-ipv6* _yes|no_
	Whether the interface should forward unicast IPv6 packets.

*forward-ipv4-mc* _yes|no_
	Whether the interface should forward multicast IPv4 packets.

*forward-ipv6-mc* _yes|no_
	Whether the interface should forward multicast IPv6 packets.

# EXAMPLES

The typical home router scenario will want to forward both IPv4 and IPv6
packets:

```
iface WAN
	use dhcp
	forward-ipv4 yes
	forward-ipv6 yes

iface LAN
	address 192.168.0.1/24
	forward-ipv4 yes
	forward-ipv6 yes
```

# AUTHORS

Ariadne Conill <ariadne@dereferenced.org>
