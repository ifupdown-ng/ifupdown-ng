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

Convention of the name is *vrrpV-XX-YY*, where *V* stands for protocol 
version: *4* for *IPv4* and *6* for *IPv6*, XX is the main interface index and 
*YY* is the *VRID* (VRRP ID).

This executor is backwards compatible with ifupdown2 configuration.

The executor will automatically set the VRF of the VRRP interfaces 
in case if main interface is part of a VRF.

See *https://www.kernel.org/doc/html/latest/networking/ipvlan.html* or 
*https://developers.redhat.com/blog/2018/10/22/introduction-to-linux-interfaces-for-virtual-networking#macvlan* or 
*http://docs.frrouting.org/en/latest/vrrp.html* for more details.

# VRRP-RELATED OPTIONS under main interface

*vrrp* _vrid_ _array_of_addresses_
	_vrid_  is the VRRP ID followed by a list of IP addresses in format 
	_IP/MASK_ _IP/MASK_ .. _IP/MASK_
	This is used to create the virtual MACVLAN 
	interface with a specific MAC address for each VRRP instance. 
	This should match in the FRRouting setup.++
*Mandadory*: _yes_

# EXAMPLES

Configure VRRP interfaces:

*R01*:
```
auto eth0
iface eth0
	address 192.168.10.2/24
	address 192.168.11.2/24
	address fc00:192:168:10::2/64
	address fc00:192:168:11::2/64
	vrrp 10 192.168.10.1 fc00:192:168:10::1
	vrrp 20 192.168.11.1 fc00:192:168:11::1
```

*R02*:
```
auto eth0
iface eth0
	address 192.168.10.3/24
	address 192.168.11.3/24
	address fc00::192:168:10:3/64
	address fc00::192:168:11:3/64
	vrrp 10 192.168.10.1 fc00::192:168:10:1
	vrrp 20 192.168.11.1 fc00::192:168:11:1
```

Then you have the FRRouting config:

*R01*
```
interface eth0
 vrrp 10 version 3
 vrrp 10 ip 192.168.10.1
 vrrp 10 ipv6 fc00:192:168:10::1
 vrrp 20 version 3
 vrrp 20 ip 192.168.11.1
 vrrp 20 ipv6 fc00:192:168:11::1
!
```

*R02*
```
interface eth0
 vrrp 10 version 3
 vrrp 10 priority 90
 vrrp 10 ip 192.168.10.1
 vrrp 10 ipv6 fc00:192:168:10::1
 vrrp 20 version 3
 vrrp 20 priority 90
 vrrp 20 ip 192.168.11.1
 vrrp 20 ipv6 fc00:192:168:11::1
!
```

The configuration will be similar to:
```
R01:~# ip address show
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc mq master red state UP group default qlen 1000
    link/ether 00:50:56:b5:d5:96 brd ff:ff:ff:ff:ff:ff
    inet 192.168.10.2/24 scope global eth0
       valid_lft forever preferred_lft forever
    inet 192.168.11.2/32 scope global eth0
       valid_lft forever preferred_lft forever
    inet6 fc00:192:168:10::2/64 scope global tentative
       valid_lft forever preferred_lft forever
    inet6 fc00:192:168:11::2/64 scope global tentative
       valid_lft forever preferred_lft forever
85: vrrp4-2-10@eth0: <BROADCAST,MULTICAST> mtu 1500 qdisc noop master red state UP group default qlen 1000
    link/ether 00:00:5e:00:01:0a brd ff:ff:ff:ff:ff:ff protodown on
    inet 192.168.10.1/32 scope global vf5b9c9a2-0a-4
       valid_lft forever preferred_lft forever
86: vrrp6-2-10@eth0: <BROADCAST,MULTICAST> mtu 1500 qdisc noop master red state UP group default qlen 1000
    link/ether 00:00:5e:00:02:0a brd ff:ff:ff:ff:ff:ff protodown on
    inet6 fc00:192:168:10::1/64 scope global tentative
       valid_lft forever preferred_lft forever
87: vrrp4-2-20@eth0: <BROADCAST,MULTICAST> mtu 1500 qdisc noop master red state UP group default qlen 1000
    link/ether 00:00:5e:00:01:14 brd ff:ff:ff:ff:ff:ff protodown on
    inet 192.168.11.1/24 scope global vf5b9c9a2-14-4
       valid_lft forever preferred_lft forever
88: vrrp6-2-20@eth0: <BROADCAST,MULTICAST> mtu 1500 qdisc noop master red state UP group default qlen 1000
    link/ether 00:00:5e:00:02:14 brd ff:ff:ff:ff:ff:ff protodown on
    inet6 fc00:192:168:11::1/64 scope global tentative
       valid_lft forever preferred_lft forever
```

Check the FRRouting configuration with:
```
R01# show vrrp summary

 Interface   VRID   Priority   IPv4   IPv6   State (v4)   State (v6)
 ----------------------------------------------------------------------
 eth0        10     100         1      1      Master       Master
 eth0        20     100         1      1      Master       Master

R01# show vrrp interface eth0

 Virtual Router ID                       10
 Protocol Version                        3
 Autoconfigured                          No
 Shutdown                                No
 Interface                               eth0
 VRRP interface (v4)                     vrrp4-2-10
 VRRP interface (v6)                     vrrp6-2-10
 Primary IP (v4)                         192.168.10.2
 Primary IP (v6)                         fc00:192:168:10::2
 Virtual MAC (v4)                        00:00:5e:00:01:0a
 Virtual MAC (v6)                        00:00:5e:00:02:0a
 Status (v4)                             Master
 Status (v6)                             Master
 Priority                                100
 Effective Priority (v4)                 100
 Effective Priority (v6)                 100
 Preempt Mode                            Yes
 Accept Mode                             Yes
 Checksum with IPv4 Pseudoheader         Yes
 Advertisement Interval                  1000 ms
 Master Advertisement Interval (v4) Rx   1000 ms (stale)
 Master Advertisement Interval (v6) Rx   1000 ms (stale)
 Advertisements Tx (v4)                  11276785
 Advertisements Tx (v6)                  11276785
 Advertisements Rx (v4)                  1
 Advertisements Rx (v6)                  1
 Gratuitous ARP Tx (v4)                  1
 Neigh. Adverts Tx (v6)                  1
 State transitions (v4)                  4
 State transitions (v6)                  4
 Skew Time (v4)                          600 ms
 Skew Time (v6)                          600 ms
 Master Down Interval (v4)               3600 ms
 Master Down Interval (v6)               3600 ms
 IPv4 Addresses                          1
 ..................................      192.168.10.1
 IPv6 Addresses                          1
 ..................................      fc00:192:168:10::1



 Virtual Router ID                       20
 Protocol Version                        3
 Autoconfigured                          No
 Shutdown                                No
 Interface                               eth0
 VRRP interface (v4)                     vrrp4-2-20
 VRRP interface (v6)                     vrrp6-2-20
 Primary IP (v4)                         192.168.11.3
 Primary IP (v6)                         fc00:192:168:11::3
 Virtual MAC (v4)                        00:00:5e:00:01:14
 Virtual MAC (v6)                        00:00:5e:00:02:14
 Status (v4)                             Master
 Status (v6)                             Master
 Priority                                100
 Effective Priority (v4)                 100
 Effective Priority (v6)                 100
 Preempt Mode                            Yes
 Accept Mode                             Yes
 Checksum with IPv4 Pseudoheader         Yes
 Advertisement Interval                  1000 ms
 Master Advertisement Interval (v4) Rx   1000 ms (stale)
 Master Advertisement Interval (v6) Rx   1000 ms (stale)
 Advertisements Tx (v4)                  11276785
 Advertisements Tx (v6)                  11276785
 Advertisements Rx (v4)                  1
 Advertisements Rx (v6)                  1
 Gratuitous ARP Tx (v4)                  1
 Neigh. Adverts Tx (v6)                  1
 State transitions (v4)                  4
 State transitions (v6)                  4
 Skew Time (v4)                          600 ms
 Skew Time (v6)                          600 ms
 Master Down Interval (v4)               3600 ms
 Master Down Interval (v6)               3600 ms
 IPv4 Addresses                          1
 ..................................      192.168.11.1
 IPv6 Addresses                          1
 ..................................      fc00:192:168:11::1

R02# show vrrp summary

 Interface   VRID   Priority   IPv4   IPv6   State (v4)   State (v6)
 ----------------------------------------------------------------------
 eth0        10     100         1      1      Backup       Backup
 eth0        20     100         1      1      Backup       Backup
 
R02# show vrrp interface eth0

 Virtual Router ID                       10
 Protocol Version                        3
 Autoconfigured                          No
 Shutdown                                No
 Interface                               eth0
 VRRP interface (v4)                     vrrp4-2-10
 VRRP interface (v6)                     vrrp6-2-10
 Primary IP (v4)                         192.168.10.3
 Primary IP (v6)                         fc00:192:168:10::3
 Virtual MAC (v4)                        00:00:5e:00:01:0a
 Virtual MAC (v6)                        00:00:5e:00:02:0a
 Status (v4)                             Backup
 Status (v6)                             Backup
 Priority                                100
 Effective Priority (v4)                 100
 Effective Priority (v6)                 100
 Preempt Mode                            Yes
 Accept Mode                             Yes
 Checksum with IPv4 Pseudoheader         Yes
 Advertisement Interval                  1000 ms
 Master Advertisement Interval (v4) Rx   1000 ms (stale)
 Master Advertisement Interval (v6) Rx   1000 ms (stale)
 Advertisements Tx (v4)                  11276785
 Advertisements Tx (v6)                  11276785
 Advertisements Rx (v4)                  1
 Advertisements Rx (v6)                  1
 Gratuitous ARP Tx (v4)                  1
 Neigh. Adverts Tx (v6)                  1
 State transitions (v4)                  4
 State transitions (v6)                  4
 Skew Time (v4)                          600 ms
 Skew Time (v6)                          600 ms
 Master Down Interval (v4)               3600 ms
 Master Down Interval (v6)               3600 ms
 IPv4 Addresses                          1
 ..................................      192.168.10.1
 IPv6 Addresses                          1
 ..................................      fc00:192:168:10::1



 Virtual Router ID                       20
 Protocol Version                        3
 Autoconfigured                          No
 Shutdown                                No
 Interface                               eth0
 VRRP interface (v4)                     vrrp4-2-20
 VRRP interface (v6)                     vrrp6-2-20
 Primary IP (v4)                         192.168.11.2
 Primary IP (v6)                         fc00:192:168:11::2
 Virtual MAC (v4)                        00:00:5e:00:01:14
 Virtual MAC (v6)                        00:00:5e:00:02:14
 Status (v4)                             Backup
 Status (v6)                             Backup
 Priority                                90
 Effective Priority (v4)                 90
 Effective Priority (v6)                 90
 Preempt Mode                            Yes
 Accept Mode                             Yes
 Checksum with IPv4 Pseudoheader         Yes
 Advertisement Interval                  1000 ms
 Master Advertisement Interval (v4) Rx   1000 ms (stale)
 Master Advertisement Interval (v6) Rx   1000 ms (stale)
 Advertisements Tx (v4)                  11276785
 Advertisements Tx (v6)                  11276785
 Advertisements Rx (v4)                  1
 Advertisements Rx (v6)                  1
 Gratuitous ARP Tx (v4)                  1
 Neigh. Adverts Tx (v6)                  1
 State transitions (v4)                  4
 State transitions (v6)                  4
 Skew Time (v4)                          600 ms
 Skew Time (v6)                          600 ms
 Master Down Interval (v4)               3600 ms
 Master Down Interval (v6)               3600 ms
 IPv4 Addresses                          1
 ..................................      192.168.11.1
 IPv6 Addresses                          1
 ..................................      fc00:192:168:11

```

# SEE ALSO

*ip-link*(8)

# AUTHORS

Adrian Ban (EasyNetDev) <devel@easynet.dev>
