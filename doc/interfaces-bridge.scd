interfaces-bridge(5)

# NAME

*interfaces-bridge* - Bridge extensions for the interfaces(5) file format

# DESCRIPTION

Linux has support for Ethernet bridging interfaces which act like an
Ethernet switch within the Linux Kernel. The following options allow
to set up Ethernet bridges and adding configured interfaces to bridges.

See *ip-link*(8) for more details about the options listed below.

# BRIDGE-RELATED OPTIONS

*bridge-ports* _list of interfaces_
	A space separated list of interfaces which should be configured
	as member interfaces of this bridge. This option must be set
	for the bridge to be configured.

*bridge-hw* _MAC address_
	Denotes the _MAC address_ the bridge should use.

*bridge-ageing* _seconds_
	Denotes the time in seconds after which a MAC address will be
	removed from the Forwarding DataBase (FDB) after not having
	seen a frame with this source address.

# SPANNING TREE RELATED BRIDGE OPTIONS

*bridge-stp* _state_
	Activates or deactivates IEEE 802.1d Spanning Tree Protocol
	(STP) support of the bridge. Valid values are _on_/_off_.

*bridge-bridgeprio* _priority_
	Sets the bridge's priority to _priority_. The priority value is
	a number between 0 and 65535. Lower priority values are better.
	The bridge with the lowest priority will be elected _root
	bridge_.

*bridge-fd* _seconds_
	Denotes the bridge forward delay in seconds. Valid values are
	between 2 and 30.

*bridge-hello* _seconds_
	Denotes the bridge hello time in seconds. Valid values are
	between 1 and 10.

*bridge-maxage* _seconds_
	Denotes the seconds until another bridge is considerd dead
	after reception of its last STP hello message. Valid values
	are between 6 and 40.

# EXAMPLES

A simple layer 2 only bridge:

```
auto br0
iface br0
	bridge-ports eth0 veth-vm1 tap0
	bridge-fd 0
	bridge-stp off
```

A bridge with layer 3 configuration:

```
auto br0
iface br0
	bridge-ports eth0 veth-vm1 tap0
	bridge-fd 0
	bridge-stp off
	#
	address 192.0.2.42/24
	address 2001:db8::42/64
```

# SEE ALSO

*ip-link*(8)
*interfaces*(5)

# AUTHORS

Maximilian Wilhelm <max@sdn.clinic>
