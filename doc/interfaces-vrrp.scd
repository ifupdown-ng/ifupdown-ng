interfaces-vrrp(5)

# NAME

*interfaces-vrrp* - VRRP extensions for the interfaces(5) file format

# DESCRIPTION

VRRP stands for Virtual Router Redundancy Protocol. This protocol 
is used to allow multiple backup routers on the same segment to take 
over operation of each others’ IP addresses if the primary router fails. 
This is typically used to provide fault-tolerant gateways to hosts 
on the segment.

Linux has support for MACVLAN from version *3.0*, but the _protodown_ 
property appears only in kernel *5.1*. Don't try to use this setup for
kernels older than *5.1*. It will not work.

To be able to use VRRP you need to run an user-space software that 
manages the Master-Backup setup. A good example is FFRouting software
which includes the VRRP daemon.

*Note*: for each type of traffic *IPv4* or *IPv6* you need a complete different
interface. For one _main interface_ where you have dual-stack traffic
you will need 2 VRRP interfaces: one for IPv4 and one for IPv6.
This setup with 2 addtitional interfaces is needed because each protocol
needs a separate MAC address for its own traffic. That why you can't use
only one VRRP interface for IPv4 and IPv6.

See https://www.kernel.org/doc/html/latest/networking/ipvlan.html or
http://docs.frrouting.org/en/latest/vrrp.html for more details.

# VRRP-RELATED OPTIONS

*vrrp-id* _vrrp id_
	The _id_ of the VRRP interface. This is used to create the
	virtual MACVLAN interface with a specific MAC address for
	each VRRP instance. This should match in the FRRouting setup.
	Mandadory: yes

*vrrp-raw-device* _main interface_
	The _main_ interface where this VRRP interface belongs.
	Mandatory: yes

*vrrp-ipv6* or *vrrp-ipv4*
	Specify for what kind of traffic is destinated this VRRP interface.
	Mandatory: no
	Default: vrrp-ipv4

# EXAMPLES

Configure VRRP interfaces:

```
auto eth0
iface eth0

auto vrrp-4-10
iface vrrp-4-10
	vrrp-ipv4
	vrrp-raw-device eth0
	vrrp-id 10

auto vrrp-6-10
iface vrrp-6-10
	vrrp-ipv6
	vrrp-raw-device eth0
	vrrp-id 10
```

Then you have the FRRouting config:

```
interface eth0
    ip address 192.168.10.2/24
    ipv6 address fc00::192:168:10:2/64
    vrrp 10
    vrrp 10 ip 192.168.10.1
    vrrp 10 ipv6 fc00::192:168:10:1

interface vrrp-4-10
    ip address 192.168.10.1/24

interface vrrp-6-10
    ipv6 address fc00::192:168:10:1/64
```

# SEE ALSO

*ip-link*(8)

# AUTHORS

Adrian Ban <devel@easynet.dev>
