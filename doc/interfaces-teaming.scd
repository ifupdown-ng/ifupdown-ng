interfaces-teaming(5)

# NAME

*interfaces-teaming* - Teaming extensions for the interfaces(5) file format

# DESCRIPTION

Link aggregation is the general term for combining network connections to
provide a logical layer. Channel teaming, Ethernet bonding, and port truncating
are synonyms that refer to the same concept.

The original implementation of this concept is known as network bonding. The new
implementation is known as network teaming. Both implementations can be used in
parallel. Teaming is an alternative to bonding, not a replacement.

The main difference between bonding and teaming is that bonding is handled
exclusively in the kernel. Teaming includes a small set of kernel modules that
provide an interface for teamd instances, but everything else is handled in user
space.

Note: because the _systemd_ kills every daemon started by networking scripts,
we have to use additional service which, after building the configuration for
teaming interface, is starts teamd@IFACE.service automatically. This way we 
assure that the daemon is started by systemd itself and is not killing it.

For _debugging_ and _troubleshooting_ the teaming for ifupdown-ng is splitted in 4 files:++
	1. Executor found in /usr/libexec/ifupdown-ng/team which just creates the virtual interface and 
collects dependencies.++
	2. Configuration builder found in /usr/sbin/teaming-build-config which is used by teamd@.service to 
build the configuration.++
	3. Helper to start the teamd daemon for the specific interface found in /usr/sbin/teaming-service.++
	4. Service file found in /lib/systemd/system/teamd@.service

The executor is very important for ifupdonw-ng mandatory create the virtual interface to be able to do the rest of the configuration
even the daemon is not started yet. Without the virtual interface the ifupdown-ng will fail to bring up subinterfaces like VLAN.++
Until the service teamd@team0.service will build the configuration and starts the daemon, ifupdown-ng will do the rest of the configuration
and it shouldn't wait after the service.

Each time you change the configuration of the interface _DO NOT_ use *ifdown team0* and then *ifup team0*.
This can cause all subinterfaces (like _VLAN_) of *team0* interface to be _deleted_ from system and can cause a lot of issues.
To reload reconfiguration of the team interface instead use *systemd reload teamd@team0.service*. It will cause a flap of the
teaming link but it will reload the new configuration without deleting the interface from system.++
This was the reason to move configuration builder outside of the executor.

To add/delete ports from interface you can use also *teamdctl* command.

The following options set up teaming/LAG interfaces with ifupdown-ng.

See https://www.kernel.org/doc/Documentation/networking/team.rst,
https://github.com/jpirko/libteam and 
https://github.com/jpirko/libteam/wiki/Infrastructure-Specification
for more information.

*Note*: There are some options that are specific for ports member.++
_DO NOT add other options which are not port member related because you can 
confuse the executor and it will try to build specific options on that interface and 
that's you don't want!_

The port member is configured by the executor when is doing the configuration stage++
Example of _non-related_ port member options: _team-runner_.++
*These options for port members are required to be queryied by the teaming service 
to be able to build the complete config. The executor is not doing any configuration 
for the port members.*

As protection, the executor checks if option _team-runner_ is configured on the interface.
In case is not configured, it will not run on that interface.++
That's why is needed to add _team-runner_ only on the teaming virtual interface and nowhere 
else in the config.

# TEAMING-RELATED OPTIONS

A teaming interface must have at least one member port set. All other options are optional.

*team-members* _array_
	Denotes the physical member interfaces to form this LAG. This is a space separated list.
	This option is required.

*team-runner* _string_

	Name of team device. The following runners are available:

		_broadcast_ — Simple runner which directs the team device to transmit packets via all ports.

		_roundrobin_ — Simple runner which directs the team device to transmits packets in a round-robin fashion.

		_random_ — Simple runner which directs the team device to transmits packets on a randomly selected port.

		_activebackup_ — Watches for link changes and selects active port to be used for data transfers.

		_loadbalance_ — To do passive load balancing, runner only sets up BPF hash function which will determine port  for
		packet  transmit. To do active load balancing, runner moves hashes among available ports trying to reach perfect
		balance.

		_lacp_ — Implements 802.3ad LACP protocol. Can use same Tx port selection possibilities as loadbalance runner.

Default: _roundrobin_

*team-notify-peers-count* _integer_
	Number of bursts of unsolicited NAs and gratuitous ARP packets sent after port is enabled or disabled.

	Default: _0_ (disabled)++
Default for activebackup runner: _1_

*team-notify-peers-interval* _integer_

	Value is positive number in milliseconds. Specifies an interval between bursts of notify-peer packets.

	Default: _0_

*team-mcast-rejoin-count* _integer_
	Number of bursts of multicast group rejoin requests sent after port is enabled or disabled.

	Default: _0_ (disabled)++
Default for activebackup runner: _1_

*team-notify-rejoin-interval* _integer_
	Value is positive number in milliseconds. Specifies an interval between bursts of multicast group rejoin requests.

	Default: _0_

Each _runner_ has a specific list of parameters or none. Please check below which parameter is used for which runner. If you set a parameter that is specific for a runner, it will be ignored by the _executor_.
Also some runners can have per port parameters configuration. For each port that needs a specific paramter you have to add in interfaces.conf a separate entry for that port adding the specific parameters.
Check the examples below.

# BROADCAST and ROUND-ROBING RUNNERS SPECIFIC OPTIONS

These 2 runners doesn't have any particular parameters to be set.

# ACTIVE-BACKUP RUNNER SPECIFIC OPTIONS

*team-runner-hwaddr-policy* _policy_
	This defines the policy of how hardware addresses of team device and port devices should be set during the team lifetime. The following are available:++
_same_all_
		All ports will always have the same hardware address as the associated team device. 
	_by_active_
		Team device adopts the hardware address of the currently active port. This is useful when the port device is not able to change its hardware address. 
	_only_active_
		Only the active port adopts the hardware address of the team device. The others have their own.

These parameters can be set on each port member:

	*team-port-prio* _integer_
		Port priority. The higher number means higher priority.

		Default: _0_
	*team-port-sticky* _bool_
		Flag which indicates if the port is sticky. If set, it means the port does not get unselected if another port with higher priority or better parameters becomes available.

		Default: _false_

# LOAD-BALANCE RUNNER SPECIFIC OPTIONS
*team-runner-tx-hash* _array_
	List of fragment types (strings) which should be used for packet Tx hash computation.++
The following are available:++
	_eth_  - Uses source and destination MAC addresses.++
	_vlan_ - Uses VLAN id.++
	_ipv4_ - Uses source and destination IPv4 addresses.++
	_ipv6_ - Uses source and destination IPv6 addresses.++
	_ip_   - Uses source and destination IPv4 and IPv6 addresses.++
	_l3_   - Uses source and destination IPv4 and IPv6 addresses.++
	_tcp_  - Uses source and destination TCP ports.++
	_udp_  - Uses source and destination UDP ports.++
	_sctp_ - Uses source and destination SCTP ports.++
	_l4_   - Uses source and destination TCP and UDP and SCTP ports.

	Default: _eth_, _ipv4_ and _ipv6_

	You can combine multiple hash policies using a space separated.

*team-runner-tx-balancer* _string_
	Name of active Tx balancer. Active Tx balancing is disabled by default. The only value available is basic.

	Default: None

*team-runner-tx-balancer-interval* _interval_
	In tenths of a second. Periodic interval between rebalancing.

	Default: _50_

# LACP RUNNER SPECIFIC OPTIONS

The following options are only valid in LACP (802.3ad) mode.

*team-lacp-active* _bool_
	If active is true LACPDU frames are sent along the configured links periodically. If not, it acts as "speak when spoken to".

	Default: _true_

*team-lacp-fast-rate* _bool_

*team-runner-tx-hash* _array_
	Same as for load-balance runner.

*team-runner-tx-balancer* 
	Same as for load-balance runner.

*team-runner-tx-balancer-interval* _integer_
	Same as for load-balance runner.


*team-lacp-sys-prio* _integer_
	System priority, value can be 0 - 65535.

	Default: _255_

*team-lacp-min-ports* _integer_
	Specifies the minimum number of ports that must be active before asserting carrier in the master interface, value can be 1 - 255.

	Default: _1_

*team-lacp-agg_select_policy* _string_
	This selects the policy of how the aggregators will be selected.

	The following are available:

		_lacp_prio_ - Aggregator with highest priority according to LACP standard will be selected. Aggregator priority is affected by per-port option lacp_prio.++
_lacp_prio_stable_ - Same as previous one, except do not replace selected aggregator if it is still usable.++
_bandwidth_ - Select aggregator with highest total bandwidth.++
_count_ - Select aggregator with highest number of ports.++
_port_options_ - Aggregator with highest priority according to per-port options prio and sticky will be selected. This means that the aggregator containing the port with the highest priority will be selected unless at least one of the ports in the currently selected aggregator is sticky.

	Default: _lacp_prio_

These parameters can be set on each port member:

	*team-port-lacp-prio* _integer_
		Port priority according to LACP standard. The lower number means higher priority.

	*team-port-lacp-key* _integer_
		Port key according to LACP standard. It is only possible to aggregate ports with the same key.

		Default: _0_

# LINK WATCH SPECIFIC OPTIONS

*team-link-watch* _string_
	Name of link watcher to be used. The following link watchers are available:

	ethtool   - Uses Libteam lib to get port ethtool state changes.++
arp_ping  - ARP requests are sent through a port. If an ARP reply is received, the link is considered to be up.++
nsna_ping - Similar to the previous, except that it uses IPv6 Neighbor Solicitation / Neighbor Advertisement mechanism. This is an alternative to arp_ping and becomes handy in pure-IPv6 environments.

	This option can be used also under each port member.

# ETHTOOL LINK WATCH SPECIFIC OPTIONS

*team-link-watch-delay-up* _integer_
	Value is a positive number in milliseconds. It is the delay between the link coming up and the runner being notified about it.

	Default: _0_

*team-link-watch-delay-down* _integer_
	Value is a positive number in milliseconds. It is the delay between the link going down and the runner being notified about it.

	Default: _0_

	These options are valid also for port members.


# ARP PING LINK WATCH SPECIFIC OPTIONS

*team-link-watch-interval* _integer_
	Value is a positive number in milliseconds. It is the interval between ARP requests being sent. 

*team-link-watch-init-wait* _integer_
	Value is a positive number in milliseconds. It is the delay between link watch initialization and the first ARP request being sent.

	Default: _0_

*team-link-watch-missed-max* _integer_
	Maximum number of missed ARP replies. If this number is exceeded, link is reported as down.

	Default: _3_

*team-link-watch-source-host* _integer_
	Hostname to be converted to IP address which will be filled into ARP request as source address.

	Default: _0.0.0.0_

*team-link-watch-target-host* _integer_
	Hostname to be converted to IP address which will be filled into ARP request as destination address. 

*team-link-watch-validate-active* _integer_
	Validate received ARP packets on active ports. If this is not set, all incoming ARP packets will be considered as a good reply.

	Default: _false_

*team-link-watch-validate-inactive* _integer_
	Validate received ARP packets on inactive ports. If this is not set, all incoming ARP packets will be considered as a good reply.

	Default: _false_

*team-link-watch-send-always* _integer_
	By default, ARP requests are sent on active ports only. This option allows sending even on inactive ports. 

	Default: _false_

	These options are valid also for port members.

# NS/NA PING LINK WATCH SPECIFIC OPTIONS

*team-link-watch-interval* _integer_
	Value is a positive number in milliseconds. It is the interval between sending NS packets.

	Default: _1000_

*team-link-watch-init-wait* _integer_
	Value is a positive number in milliseconds. It is the delay between link watch initialization and the first NS packet being sent.

*team-link-watch-missed-max* _integer_
	Maximum number of missed NA reply packets. If this number is exceeded, link is reported as down.

	Default: _3_

*team-link-watch-target-host* _integer_
	Hostname to be converted to IPv6 address which will be filled into NS packet as target address.

	These options are valid also for port members.


# EXAMPLES

A teaming using two links and LACP (802.3ad) and special parameters on port members:

```
auto team0
iface team0
	team-members eth0 eth1
	team-runner lacp
	team-tx-hash l4 ip vlan eth
	team-min-ports 1
	team-runner-tx-balancer basic
	team-link-watch ethtool
	team-link-watch-delay-up 200
	team-link-watch-delay-down 0
	#
	address 192.0.2.42/24
	address 2001:db8::42/64

iface eth0
	team-link-watch-delay-up 100

iface eth1
	team-link-watch-delay-down 50
```

A teaming using two links and LOAD-BALANCE and special parameters on port members:

```
auto team0
iface team0
	team-members eth0 eth1
	team-runner load-balance
	team-tx-hash l4 ip vlan eth
	team-min-ports 1
	team-runner-tx-balancer basic
	team-runner-tx-balancer-interval 250
	team-link-watch arp_ping
	team-link-watch-interval 250
	team-link-watch-missed-max 10
	team-link-watch-source-host 0.0.0.0
	team-link-watch-target-host 192.0.2.1
	#
	address 192.0.2.42/24
	address 2001:db8::42/64

iface eth0
	team-link-watch-interval 100
	team-link-watch-missed-max 5

iface eth1
	team-link-watch-interval 50
	team-link-watch-missed-max 10
```

# AUTHORS

Adrian Ban <devel@easynet.dev>
