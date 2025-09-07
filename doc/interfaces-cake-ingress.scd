interfaces-cake-ingress(5)

# NAME

*interfaces-cake-ingress* - CAKE ingress extensions for the
interfaces(5) file format

# DESCRIPTION

CAKE (Common Applications Kept Enhanced) is a shaping-capable queue
discipline which uses both AQM and FQ. This extension creates an IFB
(Intermediate Functional Block) interface and links it to an existing
logical interface to allow the CAKE qdisc to applied to ingress
traffic. In most applications this extension would be used with the
*interfaces-cake*(5) extension to configure the qdisc applied to
ingress traffic.

The cake-ingress extension only has one parameter, which is mandatory.

# CAKE-INGRESS OPTIONS

*cake-ingress-dev* _interface_name_
	The underlying logical interface this IFB should be
	connected to.

# EXAMPLES

## Minimal configuration

```
auto foo foo-ifb
iface foo
	use cake

iface foo-ifb
	use cake
	cake-ingress true
	use cake-ingress
	cake-ingress-dev foo
```

## Typical configuration

This configuration shapes egress traffic to 512 Mbps and ingress
traffic to 1 Gbps, while applying docsis (cable modem) overhead
compensation in both directions. As seen here the CAKE parameters
for the ingress and egress traffic can have different parameters.

```
auto foo foo-ifb
iface foo
	use cake
	cake-bandwidth 512Mbit
	cake-overhead docsis

iface foo-ifb
	use cake
	cake-bandwidth 1Gbit
	cake-overhead docsis
	cake-tins besteffort
	cake-ingress true
	use cake-ingress
	cake-ingress-dev foo
```

# SEE ALSO

*interfaces-cake*(5)
*tc-cake*(8)
