interfaces-ipv6(5)

# NAME

*interfaces-ipv6* - IPv6 configuration options for the interfaces(5) file format

# DESCRIPTION

The options in the ipv6 vocabulary control how IPv6 interface autoconfiguration
and duplicate address detection behave by adjusting kernel sysctl options as
necessary.

# IPV6 SPECIFIC KEYWORDS

Any keyword may be used inside an interface declaration block, but
the system will only respond to certain keywords by default:

*ipv6-autoconf* _number_
	Autoconfigure addresses using Prefix Information in Router Advertisements.
	Default: _1_

*ipv6-accept-ra* _number_
	Accept Router Advertisements; autoconfigure using them.
	It also determines whether or not to transmit Router Solicitations. 
	If and only if the functional setting is to accept Router Advertisements, 
	Router Solicitations will be transmitted.
	Possible values are: 0 - Do not accept Router Advertisements,
	1 - Accept Router Advertisements if forwarding is disabled,
	2 - Overrule forwarding behaviour. Accept Router Advertisements even if forwarding is enabled.
	Default: _1_

*ipv6-dad-transmits* _number_
	The amount of Duplicate Address Detection probes to send.
	Default: _1_

*ipv6-optimistic-dad* _number_
	Enable or disable optimistic Duplicate Address Detection.
	Default: _0_

# EXAMPLES

```
auto foo
iface foo
  ipv6-autoconf 0
```

# SEE ALSO

interfaces(5)
