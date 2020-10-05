interfaces-bond(5)

# NAME

*interfaces-bond* - Bonding/LAG extensions for the interfaces(5) file format

# DESCRIPTION

The Linux implementation for Ling Aggregation Groups (LAGs) is called
_bonding_, whereas a LAG interface is called _bond_. The Linux bonding
implementation supports active/passive setups, classical EtherChannels
as well as LACP (802.3ad).

The following options set up bonding/LAG interfaces with ifupdown-ng.

See https://www.kernel.org/doc/Documentation/networking/bonding.rst and
for more information.

# BOND-RELATED OPTIONS

A bond interface must have at least one member port set. All other
options are optional.

*bond-members* _list of interfaces_
	Denotes the physical member interfaces to form this LAG. For
	compatiblity to ifupdown1 and ifupdown2 _slaves_ as well as
	_bond-slaves_ are an alias for this option. This option is
	required.

*bond-mode* _mode_
	Denotes the mode for this LAG. The _mode_ can be given as string
	or as numerical value. Valid values are _balance-rr_ (0),
	_active-backup_ (1), _balance-xor_ (2), _broadcast_ (3),
	_802.3ad_ (4), _balance-tlb_ (5), _balance-alb_ (6).
	The default is _balance-rr_.

*bond-xmit-hash-policy* _policy_
	Denotes the hash policy/algorithm used to distribute packets
	across the physical links. This only applies for modes
	_balance-alb_, _balance-tlb_, _balance-xor_, and _802.3ad_.
	The _policy_ can be given as string or as numerical value.
	Valid values are _layer2_ (0), _layer3+4_ (1), _layer2+3_ (2),
	_encap2+3_ (3), and _encap3+4_ (4). The default is _layer2_.

*bond-min-links* _number_
	Denotes the minimum number of available links before turning on
	carrier.

*bond-miimon* _interval_
	Denotes the MII link monitoring frequency in milliseconds.
	This determines how often the link state of each slave is
	inspected for link failures. A value of zero disables MII
	link monitoring. The default is 0.

*bond-use-carrier* _bool_
	Denotes wether miimon uses MII or ethtool ioctls vs. the
	netif_carrier_ok() call to determine member link status.
	A value of 1 enables the use of netif_carrier_ok(), a value of
	0 will use the deprecated MII / ETHTOOL ioctls. The default
	is 1.

*bond-updelay* _delay_
	Denotes the delay in milliseconds before considering link up,
	in milliseconds. The default is 0.

*bond-downdelay* _delay_
	Denotes the delay in milliseconds before considering link down,
	in milliseconds. The default is 0.

*bond-all-slaves-active* _bool_
	Denotes wether duplicate frames (received on inactive ports)
	should be dropped (0) or delivered (1). The default is 0.

*bond-packets-per-slave* _num_packets_
	Denotes the number of packets to transmit through a member
	before moving to the next one. When set to 0 then a slave is
	chosen at random. The valid range is 0 - 65535; the default
	value is 1. This option has effect only in balance-rr mode.

*bond-lp-interval* _interval_
	Denotes the interval in seconds between sending learning packets
	to each members peer switch. The valid range is 1 - 0x7fffffff;
	the default value is 1. This option has effect only in modes
	balance-tlb and balance-alb.

*bond-resend-igmp* _number_
	Denotes the number of IGMP membership reports to send after a
	link failover happend. The valid range is 0 - 255; a value of
	0 prevents the IGMP membership report from being issued in
	response to the failover event. The default is 1.
	This option is useful for bonding modes balance-rr, active-backup
	balance-tlb and balance-alb, in which a failover can switch the
	IGMP traffic from one slave to another.

# LACP-RELATED OPTIONS

The following options are only valid in LACP (802.3ad) mode.

*bond-lacp-rate* _rate_
	Denotes the _rate_ of LACPDU requested from the peer. The _rate_
	can be given as string or as numerical value. Valid values are
	slow (0) and fast (1). The default is slow.

*bond-ad-select* _mode_
	Denotes the 802.3ad aggregation selection logic. The _mode_ can
	be given as string or as numerical value. Valid values are
	_stable_ (0), _bandwidth_ (1) and _cound_ (2). The default is
	_stable_.

*bond-ad-actor-sys-prio* _priority_
	Denotes the LACP system priority. The allowed range is 1 - 65535.
	The default value is 65535.

*bond-ad-user-port-key* _key_
	Denotes the upper 10 bits of the port-key. he values can be from
	0 - 1023. The default is 0.


# ACTIVE/BACKUP-RELATED OPTIONS

The following options are only valid in active/passive setups.

*bond-primary* _interface_
	Denotes the primary member interface The specified device will
	always be the active slave while it is available. The primary
	option is only valid for active-backup, balance-tlb and
	balance-alb mode.

*bond-primary-reselect* _policy_
	Denotes the reselection policy for the primary member interface.
	Valid values are _always_ (0), _better_ (1), and _failure_ (2).
	The default is _always_.

*bond-fail-over-mac* _mode_
	Denotes whether active-backup mode should set all member
	interfaces to the same MAC address at enslavement (the
	traditional behavior), or, when enabled, perform special
	handling of the bond's MAC address in accordance with the
	selected policy. Valid values are _none_ (0), _active_ (1),
	_follow_ (2). The default is _none_.

*bond-num-grat-arp* _count_
	Denotes the number of peer notifications (gratuitous ARPs and
	unsolicited IPv6 Neighbor Advertisements) to be issued after a
	failover event. The valid range is 0 - 255; the default is 1.

*bond-num-unsol-na* _count_
	This is an alias for _bond-num-grat-arp_

*bond-peer-notif-delay* _interval_
	Denotes the interval in milliseconds, between each peer
	notification (gratuitous ARP and unsolicited IPv6 Neighbor
	Advertisement) issued after a failover event. The default
	is 0 which means to match the value of the link monitor
	interval.

# ARP-RELATED OPTIONS

The following options configure ARP link monitoring.
The ARP monitor works by periodically checking the slave
devices to determine whether they have sent or received
traffic recently. Regular traffic is generated via ARP
probes issued for the addresses specified by the
_bond-arp-ip-target_ option.

*bond-arp-interval* _interval_
	Denotes the frequency in milliseconds to send ARP probes.

*bond-arp-ip-target* _IPv4 address_
	Denotes the IP addresses to use as ARP monitoring peers when
	_bond-arp-interval_ is > 0.

*bond-arp-validate* _mode_
	Specifies whether or not ARP probes and replies should be
	validated in any mode that supports arp monitoring, or whether
	non-ARP traffic should be filtered (disregarded) for link
	monitoring purposes. Valid values are _none_ (0), _active_ (1),
	_backup_ (2), _all_ (3), _filter_ (4), _filter_active_ (5), and
	_filter_backup_ (6). The default is _none_.

*bond-arp-all-targets* _mode_
	Denotes the number of _bond-arp-ip-targets_ that have to be
	reachable to consider the member interface to be up. Valid
	options are _any_ (0) and _all_ (1). The default is _any_.

# EXAMPLES

A bond using two links and LACP (802.3ad):

```
auto bond0
iface bond0
	bond-members eth0 eth1
	bond-mode 802.3ad
	bond-xmit-hash-policy layer3+4
	bond-min-links 1
	#
	address 192.0.2.42/24
	address 2001:db8::42/64
```

# AUTHORS

Maximilian Wilhelm <max@sdn.clinic>
