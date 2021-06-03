interfaces-tunnel(5)

# NAME

*interfaces-tunnel* - Tunnel extensions for the interfaces(5) file format

# DESCRIPTION

The following options set up tunneling interfaces with ifupdown-ng.

# TUNNEL-RELATED OPTIONS

A tunnel interface must have a mode, remote IP and a local IP or device
set, all other options are optional.

*tunnel-mode* _mode_
	Denotes the mode for this tunnel. Basically all tunnel modes supported
	by Linux / iproute2 are supported as well.  This includes but is not
	limited to _gre_/_gretap_, _ip6gre_/_ip6gretap_, _ipip_/_ip6ip_/_sit_.

*tunnel-local* _IP_
	Denotes the IP address used as the local tunnel endpoint.  According
	to the _tunnel-mode_ an IPv4 or IPv6 address has to be given.
	For compatiblity to ifupdown1 _local_ is an alias for this option.

*tunnel-local-dev* _interface_
	When the local IP address the tunnel should be established from isn't
	static and therefore might change (e.g. configured by DHCP or PPP) it
	might be desireable to just use the address configured on _interface_.
	When _tunnel-local-dev_ is given instead of _tunnel-local_ ifupdown-ng
	will try to determine the IP address set on the given _interface_ with
	respect to the address family required to set up a tunnel of the given
	_mode_ and use this to set up the tunnel.

*tunnel-remote* _IP_
	Denotes the IP address used as the remote tunnel endpoint.  According
	to the _tunnel-mode_ an IPv4 or IPv6 address has to be given.
	For compatiblity to ifupdown1 _endpoint_ is an alias for this option.

*tunnel-physdev* _interface_
	Denotes the _interface_ the encapsulated packets should be sent out by.
	This comes in handy when using VRFs to denote that the local tunnel
	endpoint should be terminated in VRF _interface_ or the VRF associated
	with _interface_.

	Note: Depending on the _mode_ of the tunnel either the VRF interface
	or the real underlay interface may have to given as _interface_.

*tunnel-ttl* _ttl_
	Denotes the TTL value to use in outgoing packets.  _ttl_ is a number in the
	range 1 - 255 whereas 0 is a special value meaning that packets inherit the
	TTL value.  The default for IPv4 tunnels is to inherit the TTL, for IPv6
	tunnels it's 64.  For compatiblity to ifupdown1 _ttl_ is an alias for this option.



# IPIP/SIT-RELATED OPTIONS

*tunnel-encap* _encap_
	Denotes the type of secondary UDP encapsulation to use for this tunnel
	if any.  Supported _encap_ values are _fou_, _gue_, and _none_.
	_fou_ indicates Foo-Over-UDP, _gue_ indicates Generic UDP Encapsulation.

# GRE-RELATED OPTIONS

*tunnel-encap* _encap_
	Denotes the type of secondary UDP encapsulation to use for this tunnel
	if any.  Supported _encap_ values are _fou_, _gue_, and _none_.
	_fou_ indicates Foo-Over-UDP, _gue_ indicates Generic UDP Encapsulation.

*tunnel-key* _key_
	Denotes the_key to used for keyed GRE to allow multiple tunnels between
	the same two endpoints.  _key_ is either a number or an IPv4 address-
	like dotted quad.  The key parameter specifies the same key to use in both
	directions.  The _tunnel-ikey_ and _tunnel-okey_ parameters specify different
	keys for input and output.  For compatiblity to ifupdown1 _key_ is an alias
	for this option.

*tunnel-hoplimit* _ttl_
	Denotes the Hop Limit value to use in outgoing packets for _ip6gre_/_ip6gretap_
	tunnels.

*tunnel-ignore-df* _bool_
	Denotes wether to enable/disable IPv4 DF suppression on this tunnel.  Normally
	datagrams that exceed the MTU will be fragmented; the presence of the DF flag
	inhibits this, resulting instead in an ICMP Unreachable (Fragmentation Required)
	message.  Enabling this attribute causes the DF flag to be ignored.

*tunnel-ikey* _key_
	Denotes the key to used for keyed GRE for packets received.  See _tunnel-key_
	for details.

*tunnel-okey* _key_
	Denotes the key to used for keyed GRE for packets sent out.  See _tunnel-key_
	for details.

*tunnel-pmtudisc* _bool_
	Denotes wether to enable/disable Path MTU Discovery on this tunnel.  It is
	enabled by default.  Note that a fixed ttl is incompatible with this option:
	tunneling with a fixed ttl always makes pmtu discovery.

*tunnel-tos* _tos_
	Denotes the TOS value to use in outgoing packets.


# EXAMPLES

A simple GRE tunnel

```
auto gre0
iface gre0
	tunnel-mode gre
	tunnel-remote 198.51.100.1
	tunnel-local 203.0.113.2
	#
	address 192.0.2.42/24
	address 2001:db8::42/64
```

A GRE tunnel where the local IP is learned from _eth0_

```
auto gre1
iface gre1
	tunnel-mode gre
	tunnel-remote 198.51.100.1
	tunnel-local-dev eth0
	#
	address 192.0.2.42/24
	address 2001:db8::42/64
```

A GRE tunnel which transfers encapasulated packets via _eth0_ which is part
of a VRF.

```
auto eth0
iface eth0
	address 203.0.113.2/24
	gateway 203.0.113.1
	vrf vrf_external

auto tun-vrf
iface tun-vrf
	tunnel-mode gre
	tunnel-remote 198.51.100.1
	tunnel-local 203.0.113.2
	tunnel-physdev eth0
	#
	address 192.0.2.42/24
	address 2001:db8::42/64

auto vrf_external
iface vrf_external
	vrf-table 1023
```

# AUTHORS

Maximilian Wilhelm <max@sdn.clinic>
