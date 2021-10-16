interfaces-vxlan(5)

# NAME

*interfaces-vxlan* - VXLAN extensions for the interfaces(5) file format

# DESCRIPTION

Virtual eXtensible LAN (VXLAN) is an overlay network to carry Layer 2 over
an IP network while accommodating a very large number of tenants. It is
defined in RFC 7348.

Be aware that VXLAN encapsulation adds 50 bytes of overhead to the IP packet
header (inner Ethernet header + VXLAN + UDP + IP). This should be taken into
consideration when setting up overlay networks, particularly on underlay
networks with a conventional 1500 byte MTU.

The following options set up VXLAN Tunnel EndPoints (VTEP) interfaces with
ifupdown-ng.

See https://www.kernel.org/doc/Documentation/networking/vxlan.rst and
https://vincent.bernat.ch/en/blog/2017-vxlan-linux for more information.

# VXLAN-RELATED OPTIONS

A VXLAN Virtual Tunnel Endpoint (VTEP) interface must an ID set. All
other options are optional.

*vxlan-id* _VNI ID_
	Denotes the VXLAN Network Identifier (VNI) ID for this interface.
	This parameter is required for VTEP interfaces.

*vxlan-physdev* _interface_
	Specifies the physical ("underlay") device to use for tunnel
	endpoint communication.  This is required for setups using
	multicast.

*vxlan-local-ip* _address_
	Specifies the source IP address to use in outgoing packets.
	For compatiblity with ifupdown2 _vxlan-local-tunnelip_ is an
	alias for this parameter.

*vxlan-peer-ips* _list of IP addresses_
	Specifies the unicast destination IP address(es) to use in outgoing
	packets when the destination link layer address is not known in
	the VXLAN device forwarding database.  This option can be used to
	form Point-to-Point as well as Point-to-Multipoint VXLAN tunnels/
	overlays depending on how many peer IPs are given.  If more than one
	IP address is given a Point-to-Multipoint overlay is being set up
	and ingress / head-end replication will be used by the Linux Kernel.
	This option cannot be used together with _vxlan-peer-group_ option.
	For compatiblity with ifupdown2 _vxlan-remoteip_ is an alias for this option
	and for compatibility with previos versions of ifupdown-ng _vxlan-remote-ip_
	is an alias for this option, too.

*vxlan-peer-group* _multicast group_
	Specifies the multicast group address to join, requires _vxlan-phsydev_
	to be set as well.  This parameter cannot be specified in combination
	with the _vxlan-peer-ips_ parameter.  For compatibility with ifupdown2
	_vxlan-svcnodeip_ is an alias for this option and for compatibility
	with previos version of ifupdown-ng _vxlan-remote-group_ is an alias, too.

*vxlan-learning* _on/off_
	Specifies if unknown source link layer addresses and IP addresses
	are entered into the VXLAN device forwarding database.

*vxlan-ageing* _seconds_
	Specifies the lifetime in seconds of FDB entries learnt by the kernel.

*vxlan-dstport* _port_
	Specifies the UDP destination port of the remote VXLAN tunnel endpoint.
	The default is 4789.

# EXAMPLES

A VTEP with multiple peers addressed via a multicast group:

```
auto vx_v1001_padcty
iface vx_v1001_padcty
	vxlan-id           655617
	vxlan-physdev	   vlan1001
	vxlan-remote-group 225.10.1.1
	#
	hwaddress f2:00:c1:01:10:01
	mtu 1560
```

The same works just fine with IPv6 in the underlay:

```
auto vx_v1400_padcty
iface vx_v1400_padcty
        vxlan-id           917505
        vxlan-physdev      vlan1400
        vxlan-peer-group   ff42:1400::1
        #
        hwaddress f2:00:0d:01:14:00
        mtu 1560
```

Note that the underlay must have an MTU of at least 1610 to
carry the encapsulated packets of the two VTEPs above.


A VTEP with one peer (unicast point-to-point configuration):

```
auto vx_ptp1
iface vx_ptp1
	vxlan-id        2342
	vxlan-local-ip  192.0.2.42
	vxlan-peer-ips  198.51.100.23
	#
	hwaddress f2:00:c1:01:10:01
```


A VTEP with multiple peers (unicast point-to-multipoint with ingress / head-end replication):

```
auto vx_her
iface vx_her
	vxlan-id	1337
	vxlan-local-ip	2001:db8:1::1
	vxlan-peer-ips  2001:db8:2::23 2001:db8:3::42 2001:db8:4::84
```

# AUTHORS

Maximilian Wilhelm <max@sdn.clinic>
