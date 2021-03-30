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

*source* _filename_
	Includes the file _filename_ as configuration data.

*source-directory* _directory_
	Includes the files in _directory_ as configuration data.

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

*point-to-point* _address_
	Sets the given IPv4 _address_ as the peer address on the
	interface. This setting only takes effect for the IPv4 address
	familiy and only makes sense in combination with a /32 netmask.
	For compatiblity with ifupdown and ifupdown2, _pointopoint_ is
	an alias for this parameter.

*link-type* _link-type_
	Denotes the link-type of the interface. When set to _dummy_,
	the interface is created as a virtual dummy interfaces.
	When set to _veth_ the interface is created as virtual veth
	interface (pair).

*veth-peer-name* _peer-name_
	Denotes the name of the veth peer interfaces. If not set
	the kernel will name the veth peer interface as _vethN_
	with N being an integer number.

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

*use* _executor_
	Designates that an executor should be used.  See _EXECUTORS_
	section for more information on executors.

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

Additional packages such as *bonding*, *bridge*, *tunnel*, *vrf* and
*vxlan* add additional keywords to this vocabulary.

# EXECUTORS

The *use* keyword designates that an _executor_ should be used.
This system is extendable by additional packages, but the
most common executors are:

*batman*
	The interface is a B.A.T.M.A.N. adv. mesh interface.
	Configuration of B.A.T.M.A.N. adv. interfaces requires the
	*batctl* untiliy to be installed.

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

*forward*
	Configures forwarding settings on the interface.

*loopback*
	Designates the interface as a loopback device.

*ppp*
	Designates the interface as a PPP device.  Configuration
	of PPP interfaces require the *ppp* and probably the *pppoe*
	packages to be installed.

*tunnel*
	The interface is a tunnel.  Configuration of tunnels
	requires the *tunnel* package to be installed.

*vrf*
	The interface is a VRF.  Configuration of VRFs requires
	the *vrf* package to be installed.

*vxlan*
	The interface is a Virtual Extensible LAN (VXLAN) tunnel
	endpoint.

*wifi*
	The interface is a Wi-Fi (IEEE 802.11) client interface.
	Configuration of the WiFi client interface requires the
	*wireless-tools* package to be installed.
	The *wpa_supplicant* package must also be installed to
	connect to hotspots using WPA-based security.

*wireguard*
	The interface is a Wireguard VPN tunnel endpoint.

Check *interfaces-<executor>(5)* for further informaton about a given
executor and available configuration parameters.

If the _auto\_executor\_selection_ ifupdown-ng.conf option is enabled,
*use* statements will automatically be added for executors when their
configuration statements are present in the interfaces file.

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

# SEE ALSO

*ifstate*(5)
*ifupdown-ng.conf*(5)
*ifup*(8)
*ifdown*(8)
*ifquery*(8)
*ifctrstat*(8)
*interfaces-batman*(5)
*interfaces-bond*(5)
*interfaces-bridge*(5)
*interfaces-forward*(5)
*interfaces-mpls*(5)
*interfaces-ppp*(5)
*interfaces-vrf*(5)
*interfaces-vxlan*(5)
*interfaces-wifi*(5)
*interfaces-wireguard*(5)

# AUTHORS

Ariadne Conill <ariadne@dereferenced.org>++
Maximilian Wilhelm <max@sdn.clinic>
