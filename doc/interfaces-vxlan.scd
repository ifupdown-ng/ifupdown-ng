interfaces-vxlan(5)

# NAME

*interfaces-vxlan* - VXLAN extensions for the interfaces(5) file format

# DESCRIPTION

Virtual eXtensible LAN (VXLAN) is an overlay network to carry Layer 2 over
an IP network while accommodating a very large number of tenants. It is
defined in RFC 7348.

Beware that VXLAN encapsulation adds an overhead of 50 bytes from witin
the overlay to the underlay (inner Ethernet frame + VXLAN + UDP + IP
haeder). Consider this when setting up underlay an overlay.

The following options allow to set up VXLAN Tunnel EndPoints (VTEPs)
interfaces with ifupdown-ng.

See https://www.kernel.org/doc/Documentation/networking/vxlan.rst and/or
https://vincent.bernat.ch/en/blog/2017-vxlan-linux for more information.

# VXLAN-RELATED OPTIONS

A VXLAN Virtual Tunnel Endpoint (VTEP) interface must an ID set. All
other options are optional.

*vxlan-id* _VNI ID_
	Denotes the VXLAN Network Identifier (VNI) ID for this interface.
	This parameter is required for a VXLAN interface.

*vxlan-physdev* _interface_
	Specifies the physical device to use for tunnel endpoint
	communication.

*vxlan-local-ip* _address_
	Specifies the source IP address to use in outgoing packets.
	For compatiblity with ifupdown2 _vxlan-local-tunnelip_ is an
	alias for this parameter.

*vxlan-remote-ip* _address_
	Specifies the unicast destination IP address to use in outgoing
	packets when the destination link layer address is not known in
	the VXLAN device forwarding database. This parameter cannot be
	specified with the _vxlan-remote-group_ parameter.
	For compatiblity with ifupdown2 _vxlan-remoteip_ is an alias for
	this parameter.

*vxlan-remote-group* _multicast group_
	Specifies the multicast IP address to join.  This parameter cannot
	be specified with the _vxlan-remote-ip_ parameter.
	For compatibility with ifupdown2 _vxlan-svcnodeip_ is an alias for
	this parameter.

*vxlan-learning* _on/off_
	Specifies if unknown source link layer addresses and IP addresses
	are entered into the VXLAN device forwarding database.

*vxlan-ageing* _seconds_
	Specifies the lifetime in seconds of FDB entries learnt by the kernel.

*vxlan-dstport* _port_
	Specifies the UDP destination port to communicate to the remote VXLAN
	tunnel endpoint. The default is 4789.

# EXAMPLES

A VTEP with multiple peers addressed via a multicast group:

...
auto vx_v1001_padcty
iface vx_v1001_padcty
        vxlan-id           655617
	vxlan-physdev	   vlan1001
	vxlan-remote-group 225.10.1.1
	#
	hwaddress f2:00:c1:01:10:01
	mtu 1560
...

Note that the underlay must have an MTU of at least 1610 to
carry the encapsulated packets.


A VTEP with one peer (point-to-point configuration):

...
auto vx_ptp1
iface vx_ptp1
        vxlan-id        2342
	vxlan-local-ip  192.0.2.42
	vxlan-remote-ip 198.51.100.23
	#
	hwaddress f2:00:c1:01:10:01
...

# AUTHORS

Maximilian Wilhelm <max@sdn.clinic>
