interfaces-vrf(5)

# NAME

*interfaces-vrf* - VRF extensions for the interfaces(5) file format

# DESCRIPTION

Linux has support for Virtual Routing and Forwarding (VRF) instances
since Kernel >= 4.4. The following options allow to set up VRFs and
adding configured interfaces to VRFs.

Note that in the Linux Kernel VRFs are represented as network interfaces,
too. See https://www.kernel.org/doc/Documentation/networking/vrf.rst for
more details.

# VRF-RELATED OPTIONS

*vrf-table* _table id_
	The _id_ of the kernel routing table associated with this
	VRF interface. This parameter indicates that the interface
	where it is specified shall be a VRF.

*vrf* _vrf interface_
	The _vrf_ the interface should be assigned to. This parameter
	is specified on regular interfaces which should be within the
	given _vrf_.

# EXAMPLES

A VRF interface:

```
auto vrf_external
iface vrf_external
	vrf-table 1023
```

A regular interface which should be within a VRF:

```
auto eth0
iface eth0
	address 192.2.0.42/24
	address 2001:db8::42/64
	gateway 192.2.0.1
	gateway 2001:db::1
	vrf vrf_external
```

# SEE ALSO

*ip-vrf*(8)
*ip-link*(8)

# AUTHORS

Maximilian Wilhelm <max@sdn.clinic>
