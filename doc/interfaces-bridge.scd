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
	for the bridge to be configured. Set it to _none_ to create a
	bridge with no ports.

*bridge-hw* _MAC address_
	Denotes the _MAC address_ the bridge should use.

*bridge-ageing* _seconds_
	Denotes the time in seconds after which a MAC address will be
	removed from the Forwarding DataBase (FDB) after not having
	seen a frame with this source address.

*bridge-vlan-aware* _bool_
	Denotes wether or not the bridge should be aware of 802.1q VLANs.
	_bool_ can be given as _yes_/_no_ or _0_/_1_.  The defaul is _no_.
	See related options for configuring vlan-aware bridges, below.

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

# OPTIONS FOR VLAN-AWARE-BRIDGES

The following options only have an effect on vlan-aware bridges and
their ports.

All settings can be applied on the bridge interface itself and all member
port iface stanzas.  If applied on the bridge interface they take effect
for the bridge interface itself and might be inherited to _bridge-ports_
depending on the compatibility settings configured in *ifupdown-ng.conf*(5).

Configuring VLAN options on the bridge interface might be required for
setting up a VLAN interface to one of the VLANs carried within the bridge.
See the EXAMPLES section for an example for this scenario.

See *ifupdown-ng.conf*(5) for more information about compatiblity settings
mentioned below.

*bridge-access* _vlan ID_
	Configure the given _vlan ID_ for untagged ingress and egress
	on this interface.  The common description for this kind of
	configuration is called "access port".

*bridge-pvid* _vlan ID_
	Denotes the _vlan ID_ to considered a PVID at ingress.
	Any untagged frames received on this interface will be
	assigned to this _vlan ID_.  The default PVID is _1_.

	If compatibility to ifupdown2 bridge port inheritance is active
	a _bridge-pvid_ set on the bridge will be inherited to any
	interface configured in _bridge-ports_ without a _bridge-pvid_ set.

*bridge-vids* _list of vlan IDs_
	Denotes the space separated list of VLANs to be allowed tagged
	ingress/egress on this interface.

	If compatibility to ifupdown2 bridge port inheritance is active
	a _bridge-vids_ set on the bridge will be inherited to any
	interface configured in _bridge-ports_ without _bridge-vids_ set.

*bridge-allow-untagged* _bool_
	Denotes wether or not the bridge should allow untagged frames on
	ingress as well as egress. If set to _no_ untagged frames will be
	droppped on ingress and none will be sent. _bool_ can be given as
	_yes_/_no_ or _0_/_1_.  The defaul is _yes_.

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

A layer 2 only vlan-aware bridge:

```
auto bond0
iface bond0
	bond-members eth0 eth1
	bridge-vids 23 42 84 1337

auto br0
iface br0
	bridge-ports bond0
```

A vlan-aware bridge with a VLAN interface on top:

```
auto eth0
iface eth0
	bridge-vids 23 42 84 1337

auto br0
iface br0
	bridge-ports eth0
	bridge-vlan-aware yes
	bridge-vids 42

auto vlan42
iface vlan42
	vlan-raw-device br0
	#
	address 192.0.2.42/24
	address 2001:db8::42/64
```

# SEE ALSO

*interfaces*(5)
*ifupdown-ng.conf*(5)
*ip-link*(8)
*bridge*(8)

# AUTHORS

Maximilian Wilhelm <max@sdn.clinic>
