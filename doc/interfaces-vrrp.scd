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
For this reason 2 addtitional interfaces are needed because each protocol 
needs its separate MAC address for that specific traffic. That's why you 
can't use only one VRRP interface for both IPv4 and IPv6 traffic.

Convention of the name is *vXXXXXXXX-YY-V*, where *XXXXXXXX* is CRC32 value in HEX 
calculated from main *interface name*. *YY* is the *VRRP ID*. *V* stands for protocol 
version: *4* for *IPv4* and *6* for *IPv6*.

There are 2 ways to use CRC32: crc32sum tool that is provider with ifupdown-ng 
or cksum provided by coreutils. The script is searching first for crc32sum and 
then for cksum.

The tools are having compatible output. crc32sum provided in ifupdown-ng is a stripped 
version of cksum from coreutils.

This executor takes care about the VRF of the VRRP interfaces in case if 
main interface should be under a VRF.

See *https://www.kernel.org/doc/html/latest/networking/ipvlan.html* or 
*https://developers.redhat.com/blog/2018/10/22/introduction-to-linux-interfaces-for-virtual-networking#macvlan* or 
*http://docs.frrouting.org/en/latest/vrrp.html* for more details.

# VRRP-RELATED OPTIONS under main interface

*vrrp-id* _vrrp id array_
	List of VRRP _id_. This is used to create the virtual MACVLAN 
	interface with a specific MAC address for each VRRP instance. 
	This should match in the FRRouting setup.++
*Mandadory*: _yes_

*vrrp-address* _array of addresses_
	The format should be: *VRRP_ID* _IP/MASK_ _IP/MASK_ _.._ *VRRP_ID* _IP/MASK_ _.._++
Optionally you can assign one or more IPv4 or IPv6 to each VRRP interface.++
But because each main interface can have multiple VRRPs, we need to specify 
	on which VRRP pair we want the IP address. The script automatically detects 
	the type of IP version.++
*Mandatory*: _no_++
*Default*: _none_

# EXAMPLES using crc32sum tool and FRRouting to configure the IP interfaces

Configure VRRP interfaces:

```
auto eth0
iface eth0
	vrrp-id 10 20
```

Then you have the FRRouting config:

*R01*
```
interface eth0
    ip address 192.168.10.2/24
    ip address 192.168.11.2/24
    ipv6 address fc00::192:168:10:2/64
    ipv6 address fc00::192:168:11:2/64
    vrrp 10
    vrrp 10 ip 192.168.10.1
    vrrp 10 ipv6 fc00::192:168:10:1
    vrrp 20
    vrrp 20 ip 192.168.11.1
    vrrp 20 ipv6 fc00::192:168:11:1
!
interface v43bfa0f3-0a-4
    ip address 192.168.10.1/24
!
interface v43bfa0f3-14-4
    ip address 192.168.11.1/24
!
interface v43bfa0f3-0a-6
    ipv6 address fc00::192:168:10:1/64
!
interface v43bfa0f3-14-6
    ipv6 address fc00::192:168:11:1/64
```

*R02*
```
interface eth0
    ip address 192.168.10.3/24
    ip address 192.168.11.3/24
    ipv6 address fc00::192:168:10:3/64
    ipv6 address fc00::192:168:11:3/64
    vrrp 10
    vrrp 10 priority 90
    vrrp 10 ip 192.168.10.1
    vrrp 10 ipv6 fc00::192:168:10:1
    vrrp 20
    vrrp 20 priority 90
    vrrp 20 ip 192.168.11.1
    vrrp 20 ipv6 fc00::192:168:11:1
!
interface v43bfa0f3-0a-4
    ip address 192.168.10.1/24
!
interface v43bfa0f3-14-4
    ip address 192.168.11.1/24
!
interface v43bfa0f3-0a-6
    ipv6 address fc00::192:168:10:1/64
!
interface v43bfa0f3-14-6
    ipv6 address fc00::192:168:11:1/64
```

The compute CRC32 of *eth0* with crc32sum or cksum is 0x43bfa0f3, "0a" means 10 (0x0a) in hex and "14" is 20 (0x14) in hex.

# EXAMPLES using crc32sum or cksum tool and vrrp-address option.
```
auto red
iface red
    vrf-table 300

auto eth0
iface eth0
    address 192.168.10.2/24
    address 192.168.11.2/24
    address 192.168.12.2/24
    address 192.168.13.2/24
    address fc00:192:168:1::2/64
    address fc00:192:168:2::2/64
    vrf red
    vrrp-id 10 20
    vrrp-address 10 192.168.10.1/24 192.168.12.1/24 fc00:192:168:1::1/64 20 192.168.11.1/24 192.168.13.1/24 fc00:192:168:2::1/64
```

The configuration will be similar to:
```
R01:~# ip address show
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc mq master red state UP group default qlen 1000
    link/ether 00:50:56:b5:d5:96 brd ff:ff:ff:ff:ff:ff
    inet 192.168.10.2/24 scope global eth0
       valid_lft forever preferred_lft forever
    inet 192.168.11.2/24 scope global eth0
       valid_lft forever preferred_lft forever
    inet 192.168.12.2/24 scope global eth0
       valid_lft forever preferred_lft forever
    inet 192.168.13.2/24 scope global eth0
       valid_lft forever preferred_lft forever
    inet6 fc00:192:168:2::2/64 scope global tentative
       valid_lft forever preferred_lft forever
    inet6 fc00:192:168:1::2/64 scope global tentative
       valid_lft forever preferred_lft forever
85: v43bfa0f3-0a-4@eth0: <BROADCAST,MULTICAST> mtu 1500 qdisc noop master red state UP group default qlen 1000
    link/ether 00:00:5e:00:01:0a brd ff:ff:ff:ff:ff:ff protodown on
    inet 192.168.10.1/24 scope global vf5b9c9a2-0a-4
       valid_lft forever preferred_lft forever
    inet 192.168.12.1/24 scope global vf5b9c9a2-0a-4
       valid_lft forever preferred_lft forever
86: v43bfa0f3-0a-6@eth0: <BROADCAST,MULTICAST> mtu 1500 qdisc noop master red state UP group default qlen 1000
    link/ether 00:00:5e:00:02:0a brd ff:ff:ff:ff:ff:ff protodown on
    inet6 fc00:192:168:1::1/64 scope global tentative
       valid_lft forever preferred_lft forever
87: v43bfa0f3-14-4@eth0: <BROADCAST,MULTICAST> mtu 1500 qdisc noop master red state UP group default qlen 1000
    link/ether 00:00:5e:00:01:14 brd ff:ff:ff:ff:ff:ff protodown on
    inet 192.168.11.1/24 scope global vf5b9c9a2-14-4
       valid_lft forever preferred_lft forever
    inet 192.168.13.1/24 scope global vf5b9c9a2-14-4
       valid_lft forever preferred_lft forever
88: v43bfa0f3-14-6@eth0: <BROADCAST,MULTICAST> mtu 1500 qdisc noop master red state UP group default qlen 1000
    link/ether 00:00:5e:00:02:14 brd ff:ff:ff:ff:ff:ff protodown on
    inet6 fc00:192:168:2::1/64 scope global tentative
       valid_lft forever preferred_lft forever
```

# SEE ALSO

*ip-link*(8)

# AUTHORS

Adrian Ban (EasyNetDev) <devel@easynet.dev>
