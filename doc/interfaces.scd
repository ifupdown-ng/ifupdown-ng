interfaces(5)

# NAME

*/etc/network/interfaces* - interface configuration database

# DESCRIPTION

The */etc/network/interfaces* file is used to specify how network
interfaces are configured.  The file is processed by *ifquery*(8),
*ifup*(8) and *ifdown*(8) to introspect and change system state.

In most cases, syntax from legacy implementations is supported as
well, but that syntax is not discussed here.

# FILE SYNTAX

The interface configuration database is composed of a series of
stanzas.  Hash symbols designate comments, which are ignored by
the system.

A stanza is a collection of triples, where a triple is a key and
value combination that is related to an *object*.  Triples which
are not associated with an *object* are considered to be part
of the root of the configuration tree.

The following is a simple example of a stanza:

```
auto eth0
iface eth0
    address 203.0.113.2/24
    gateway 203.0.113.1
```

This stanza defines an interface named *eth0* which is configured
with an address of *203.0.113.2* and gateway of *203.0.113.1*.

# SUPPORTED KEYWORDS FOR UNASSOCIATED TRIPLES

*auto* _object_
	Designates that _object_ should be automatically configured
	by the system when appropriate.

*iface* _object_
	Begins a new declaration for _object_.  Any child keyword
	associated with the declaration will be stored inside
	_object_.

*template* _object_
	Begins a new declaration for _object_, like *iface*, except
	that _object_ is defined as a *template*.

# SUPPORTED KEYWORDS FOR OBJECT TRIPLES

Any keyword may be used inside an interface declaration block, but
the system will only respond to certain keywords by default:

*address* _address_
	Associates an IPv4 or IPv6 address in CIDR notation with
	the parent interface.

*gateway* _address_
	Associates an IPv4 or IPv6 address with the parent interface
	for use as a default route (gateway).

*netmask* _netmask_
	Associates a fallback netmask with the parent interface for
	addresses which do not have a CIDR length set.  This option
	is for backwards compatibility and should not be used in new
	deployments.

*link-type* _link-type_
	Denotes the link-type of the interface. When set to _dummy_,
	the interface is created as a virtual dummy interfaces.

*alias* _alias_
	Sets the given alias on the interface.

*requires* _interfaces_...
	Designates one or more required interfaces that must be
	brought up before configuration of the parent interface.
	Interfaces associated with the parent are taken down at
	the same time as the parent.

*inherit* _object_
	Designates that the configured interface should inherit
	configuration data from _object_.  Normally _object_
	must be a *template*.

*use* _option_
	Designates that an option should be used.  See _OPTIONS_
	section for more information on options.

*pre-down* _command_
	Runs _command_ before taking the interface down.

*down* _command_
	Runs _command_ when the interface is taken down.

*post-down* _command_
	Runs _command_ after taking the interface down.

*pre-up* _command_
	Runs _command_ before bringing the interface up.

*up* _command_
	Runs _command_ when the interface is brought up.

*post-up* _command_
	Runs _command_ after bringing the interface up.

Additional packages such as *bonding*, *bridge*, *tunnel* and
*vrf* add additional keywords to this vocabulary.

# OPTIONS

The *use* keyword designates that an _option_ should be used.
This system is extendable by additional packages, but the
most common options are:

*bond*
	The interface is a bonded interface.  Configuration
	of bonded interfaces requires the *bonding* package
	to be installed.

*bridge*
	The interface is an ethernet bridge.  Configuration
	of ethernet bridges requires the *bridge* package
	to be installed.

*dhcp*
	Use a DHCP client to learn the IPv4 address of an
	interface.

*loopback*
	Designates the interface as a loopback device.

*tunnel*
	The interface is a tunnel.  Configuration of tunnels
	requires the *tunnel* package to be installed.

*vrf*
	The interface is a VRF.  Configuration of VRFs requires
	the *vrf* package to be installed.

# EXAMPLES

Configure a bridge interface *br0* with *bond0* attached to it,
which is a failover between *eth0* and *eth1*.  This requires
the *bonding* and *bridge* packages to be installed:

```
auto br0
iface br0
    use bridge
    requires bond0
    address 203.0.113.2/24
    gateway 203.0.113.1

iface bond0
    use bond
    requires eth0 eth1
    bond-mode 802.3ad
    bond-xmit-hash-policy layer2+3
```

Configure a network interface to use DHCP to learn its IPv4
address:

```
auto eth0
iface eth0
    use dhcp
```

# AUTHORS

Ariadne Conill <ariadne@dereferenced.org>
Maximilian Wilhelm <max@sdn.clinic>
