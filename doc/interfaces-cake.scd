interfaces-cake(5)

# NAME

*interfaces-cake* - CAKE extensions for the interfaces(5) file format

# DESCRIPTION

CAKE (Common Applications Kept Enhanced) is a shaping-capable queue
discipline which uses both AQM and FQ.

All parameters for CAKE are optional, though most applications would
benefit by specifying a bandwidth estimate. The overhead and RTT
parmaters can also provide meaningful tuning for many applications,
if you have information about the underyling link technology.

See *tc-cake*(8) for details on all parameters, as well as guidance
about their selection in different applications.

If you're not sure what to do try setting the bandwidth parameter to
the link rate of your modem, or to the rate you find with an egress
speed test.

# CAKE OPTIONS

*cake-args* _complete_cake_arguments_
	If provided this set of arguments is passed directly to
	*tc-cake*(8). If you already know your CAKE config you can
	paste it here without modification. All other parameters
	are ignored if this parameter is set.

*cake-bandwidth* _rate_
	The transmission rate of the interface, in floating-point
	bits per second, with an optional unit (124.5Mbit).
	There is no default, which is equivalent to disabling
	the overall bandwidth limit

*cake-rtt* _time_
	Round-trip transmission delay for typical traffic on this
	interface. Default: internet

*cake-tins* _queue_set_
	Specifies the set of queues and related tuning CAKE will
	use. Default: diffserv3

*cake-isolation* _isolation_type_
	Specifies the data used to hash flows within each queue.
	Default: triple-isolate

*cake-nat* _boolean_
	Do a NAT lookup before flow hashing. Can improve fairness
	among hosts behind NAT translations performed by this
	router. Default: false

*cake-wash* _boolean_
	Clear all extra diffserv (but not ECN bits) after priority
	queuing has taken place. Default: false

*cake-split_gso* _boolean_
	Disable GSO (General Segmentation Offload) to improve
	latency. Many applications under 10Gbps should use the
	default: true

*cake-ack* _ack_filter_type_
	Select the type of ACK filtering used in CAKE. This can
	be useful on asymmetric links where ACKs are likely to
	queued. Default: no-ack-filter

*cake-memlimit* _bytes_
	Limit the overall memory use of CAKE for this interface.
	There is no default

*cake-fwmark* _mask_
	Override tin selection with firewall marks and this mask.
	There is no default

*cake-atm* _atm_compensation_type_
	Apply ATM or PTM compentation when calculating overhead.
	Default: noatm

*cake-overhead* _bytes_
	Per-packet overhead, used to derate the overall bandwidth
	to compenstate for the difference between the in-kernel
	packet size and the on-wire packet size. Default: raw,
	which uses the kernel's packet size without compensation

*cake-mpu* _bytes_
	Minimum on-wire packet length, used to derate the overall
	bandwidth for packets where the calculated raw size
	+ overhead would be smaller than this number. There is no
	default

*cake-ingress* _boolean_
	Enable ingress mode. Typically this would be used with an
	IFB device to apply shaping to ingress traffic.
	Default: false

# EXAMPLES

## Minimal configuration

```
auto foo
iface foo
	use cake
```

## Typical configurations

This configuration shapes ingress traffic to 1 Gbps while applying
docsis (cable modem) overhead compensation

```
auto foo
iface foo
	use cake
	cake-bandwidth 1Gbit
	cake-overhead docsis
```

This configuration shapes ingress traffic up to 1 Gbps while applying
ethernet + ether_vlan (Ethernet with tagged VLAN) overhead
compensation and ACK filtering

```
auto foo
iface foo
	use cake
	cake-args bandwidth 1Gbit ethernet ether-vlan nat ack-filter
```

## All available options

```
auto foo
iface foo
	use cake
	cake-bandwidth 1Gbit
	cake-rtt internet
	cake-tins diffserv3
	cake-isolation triple-isolate
	cake-nat false
	cake-wash false
	cake-split_gso true
	cake-ack no-ack-filter
	cake-memlimit 20000K
	cake-fwmark 0x0
	cake-atm noatm
	cake-overhead raw
	cake-mpu 84
	cake-ingress false
```

# SEE ALSO

*interfaces-cake-ingress*(5)
*tc-cake*(8)
