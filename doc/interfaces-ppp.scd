interfaces-ppp(5)

# NAME

*interfaces-ppp* - PPP extensions for the interfaces(5) file format

# DESCRIPTION

The Point-to-Point Protocol (PPP) usually is used for dial-up lines,
most common for over Digital Subscriber Lines (DSL) to connect to an
Internet Service Provider (ISP). The following options allow to set
up PPP dial-up connections.

# PPP-RELATED OPTIONS

*ppp-provider* _provider_
	Denotes the file name of the _provider_ configuration file
	within the _/etc/ppp/peers/_ directory which should be used
	to set up the PPP connection.

*ppp-physdev* _interfaces_
	Denotes the physical (underlay) interface which is used to
	set up the PPP connection.

# EXAMPLES

A PPP connection to _local-ISP_:

```
auto ppp0
iface ppp0
	ppp-provider local-ISP
```

# AUTHORS

Maximilian Wilhelm <max@sdn.clinic>
