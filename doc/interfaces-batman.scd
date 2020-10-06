interfaces-batman(5)

# NAME

*interfaces-batman* - B.A.T.M.A.N. adv. extensions for the interfaces(5)
file format

# DESCRIPTION

Better Approach To Mobile Ad-Hoc Networking (B.A.T.M.A.N.) advanced is
a mesh protocol which provides an Ethernet overlay network over an
Ethernet underlay. The overlay interface is called _meshif_ whereas
underlay interfaces are called _hardif_.

It's supported in the Linux kernel and thus available in many Linux
environments. The ifupdown-ng exectuor relies on the *batctl* tool
being installed. Support for setting interface based hop-penalties
required Linux Kernel 5.8 or later.

B.A.T.M.A.N. adv. adds 30-60 bytes of encapsulation overhead depending
on wether netword coding is activated or not. This should be taken into
consideration when setting up overlay networks, particularly on underlay
networks with a conventional 1500 byte MTU.

See https://www.open-mesh.org/projects/open-mesh/wiki for more details
and updates.

The following options allow to set up B.A.T.M.A.N. adv. interfaces.

# BATMAN-RELATED OPTIONS

*batman-ifaces* _list of interfaces_
	Specifies the underlay interfaces (hardifs) which should be
	configured for the B.A.T.M.A.N. adv. meshif defined within
	the iface stanza.

*batman-hop-penalty* _hop-penalty_
	The _hop-penalty_ defines the cost of traversing a node or an
	interface. The _hop-penalty_ is a numeric value between 0 and
	255. Historically a _hop-penalty_ could only be set on a meshif,
	since B.A.T.M.A.N adv. v2020.3 (included in Kernel 5.8) it can
	also be set on a per-interfaces (hardif) basis.

*batman-gw-mode* _gw-mode_
	Denotes the gateway mode which controls the role this node will
	play within this B.A.T.M.A.N. adv. instance. The mode can be
	_off_, _client_, or _server_.

*batman-distributed-arp-table* _mode_
	Activates or deactivates the Distributed ARP table (DAT) within
	this B.A.T.M.A.N. adv. instance. Valid values are _enable_ and
	_disable_.

*batman-multicast-mode* _mode_
	Activates or deactivates the multicast mode of this B.A.T.M.A.N.
	adv. instance. Valid values are _enable_ and _disable_.

# EXAMPLES

A B.A.T.M.A.N. adv. _meshif_:

```
auto bat-pad-cty
iface bat-pad-cty
	batman-ifaces dummy-pad-cty vlan1234
	batman-hop-penalty 5
	#
	hwaddress f2:00:c1:01:00:00
	mtu 1500
```

A B.A.T.M.A.N. adv. member interfaces (_hardif_):

```
auto vlan1234
iface vlan1234
	mtu 1560
	batman-hop-penalty 10
```

# SEE ALSO

*batctl*(8)

# AUTHORS

Maximilian Wilhelm <max@sdn.clinic>
