interfaces-vxlan(5)

# NAME

*interfaces-vlan* - VLAN configuration for the interfaces(5) file format

# DESCRIPTION

VLAN or virtual local area network (VLAN) is any broadcast domain that is 
partitioned and isolated in a computer network at the data link layer (OSI layer 2).

VLAN has two types of encapsulation: 802.1Q and 802.1ad.

Under IEEE 802.1Q, the maximum number of VLANs on a given Ethernet network is 4094 
(4096 values provided by the 12-bit VID field minus reserved values at each end of 
the range, 0 and 4,095). This does not impose the same limit on the number of 
IP subnets in such a network since a single VLAN can contain multiple IP subnets. 
IEEE 802.1ad extends the number of VLANs supported by adding support for multiple, 
nested VLAN tags. IEEE 802.1aq (Shortest Path Bridging) expands the VLAN limit to 
16 million. Both improvements have been incorporated into the IEEE 802.1Q standard. 

The following options set up VLAN interfaces with ifupdown-ng.

# VLAN-RELATED OPTIONS

A VLAN Virtual Local Area Network interface must have a VLAN ID set. All
other options are optional.

VLAN accepted formats via interface format: \*.\*, vlan\*, \*

For \*.\* format, vlan-raw-device and vlan-id are extracted from the interface 
name where vlan-raw-device is the characters group before dot and vlan-id is
the number after dot.

For vlan\* format, vlan-id is extracted after "vlan" and you need the option
vlan-raw-device to be mandatory configured.

For \* format, you can choose any name you want for VLAN interface but you need
to configure mandatory the options vlan-raw-device and vlan-id.

*vlan-id* _id_
	Specifies the VLAN Identifier to use. Note that numbers with a 
	leading " 0 " or " 0x " are interpreted as octal or hexadecimal, 
	respectively. This is required for setups where you are using general
	VLAN interface naming.

*vlan-raw-device* _interface_
	Specifies the physical ("underlay") device to use for VLAN interface.
	This is required for setups where you are not using VLAN i
	nterface naming \*.\*

*vlan-protocol* _protocol_
	Specifies the VLAN protocol: 802.1Q or 802.1ad. Protocol QinQ is an 
	alias for 802.1ad.
	Protocol 802.1Q is by default the protocol used for creation of the VLAN 
	interface.
	For 802.1ad, aka QinQ, you must specify VLAN protocol to 802.1.ad.
	If you want to use 802.1Q over 802.1ad, you must create a separate VLAN
	interface. Check examples.

	Note for MTU:
	Be careful to MTU when you are using QinQ protocol. Most of the NICs will
	add automatically 4 or 8 bytes to the outgoing packates, but some not.
	If you create a VLAN 802.1Q above 802.1ad interface, you need to 
	increase 802.1ad MTU at least by 4 or 8 bytes, depending on you hardware.
	For Intel i210 or i350 both VLAN tags are added to the outgoing packet, but
	if you have a switch between the devices, check for the ports to have
	a minumum 1522 bytes MTU, otherwise will get the frames dropped.

	Also a VLAN 802.1ad can be used as bridge port with VLAN filtering and it
	can transport 802.1Q traffic over 802.1ad interface. Check examples.

*vlan-reorder-hdr* _on/off_
	If reorder_hdr is on then VLAN header will be not inserted immediately 
	but only before passing to the physical device (if this device does not s
	upport VLAN offloading), the similar on the RX direction - by default 
	the packet will be untagged before being received by VLAN device. 
	Reordering allows one to accelerate tagging on egress and to hide VLAN header 
	on ingress so the packet looks like regular Ethernet packet, at the same time 
	it might be confusing for packet capture as the VLAN header does not exist 
	within the packet.
	
	VLAN offloading can be checked by ethtool(8):
	
	ethtool -k <phy_dev> | grep tx-vlan-offload
	
	where <phy_dev> is the physical device to which VLAN device is bound. 

*vlan-gvrp* _on/off_
	Specifies whether this VLAN should be registered using GARP VLAN Registration Protocol.

*vlan-mvrp* _on/off_
	Specifies whether this VLAN should be registered using Multiple VLAN Registration Protocol.

*vlan-loose-binding* _on/off_
	Specifies whether the VLAN device state is bound to the physical device state.

*vlan-bridge-binding* _on/off_
	Specifies whether the VLAN device link state tracks the state of bridge ports that are
	members of the VLAN.

# EXAMPLES

Creating a VLAN using diffrent interface formats:

```
auto eth0.30
iface eth0.30
        address 192.168.30.1/24

auto vlan40
iface vlan40
        vlan-raw-device eth0
        address 192.168.40.1/24

auto myiface1
iface myiface1
        vlan-id 50
        vlan-raw-device eth0
        address 192.168.50.1/24
```

To create a 802.1ad (QinQ) interface and then attach a 802.1Q traffic:

```
auto eth0.30
iface eth0.30
	vlan-protocol 802.1ad

auto eth0-30.100
iface eth0-30.100
	vlan-raw-device eth0.30
	vlan-protocol 802.1Q
	address 192.168.100.1/24

auto eth0-30.101
iface eth0-30.101
	vlan-protocol 802.1Q
	address 192.168.101.1/24

```
You can omit vlan-protocol 802.1Q in interface config eth0-30.100 because ifupdown-ng 
will use 802.1Q protocol by default.

Now we have 3 VLAN interfaces: one 802.1ad QinQ VLAN named eth0.30 that encapsulates everything and
the other 2 which are 802.1Q, 100 and 101.

To use 802.1ad QinQ as trasport port you can set a bridge with VLAN on top like this:

```
auto eth0
iface eth0
        bridge-vids 23 42 84 1337

auto eth1.100
iface eth1.100
        vlan-protocol 802.1ad
        bridge-vids 23 42 84 1337

auto svi1
iface svi1
        bridge-vlan-aware
        bridge-ports eth0 eth1.100
        bridge-vlan-aware yes
        bridge-vids 42

auto svi1.42
iface svi1.42
        address 192.0.2.42/24
        address 2001:db8::42/64
```

Short explanation: We create a VLAN 802.1ad over eth1 which will be a port of the svi1 bridge together with eth0.
Tagger income traffic on ethernet eth0 will be processed by the svi1 bridge and then will be forwarded 
to eth1.100 internet. Then eth1.100 will encapsulate any of the vlans 23, 42, 84 or 1337 to a VLAN 802.1ad 
with tag 100 and it will send to physical eth1 interface.

One reason is when you want to transport the VLAN between 2 routers and use VRRP. In this case if you are 
not connecting the VLANs which are using VRRP between 2 routers and you are using only the uplinks 
to the switches for VRRP, then if the connection between 2 switches breaks will will end in a split-brain 
situation: both routers will bring up VRRP and the income traffic on R01 can't reach the hosts directly 
connected to R02 and vice-versa.

But also if you need MPLS redundancy over the R01-R02 link, if you are using physical port as bridge port, STP/RSTP
will block this link.

Using 802.1ad transport, the STP/RSTP will be send via 802.1ad interface and not over the physical interface.
So STP/RSTP will block eth1.100 port, and not the whole traffic over eth1, leaving eth1 as L3 interface for
MPLS or other stuff.

# TODO

Implement mstpd daemon for STP/RSTP protocol in bridge executor.

# AUTHORS

Maximilian Wilhelm <max@sdn.clinic>
EasyNetDev <devel@easynet.dev>
