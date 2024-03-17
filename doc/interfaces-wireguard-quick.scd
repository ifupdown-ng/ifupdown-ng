interfaces-wireguard-quick(5)

# NAME

*interfaces-wireguard-quick* - wg-quick extensions for the interfaces(5) file format

# DESCRIPTION

Wireguard is a comtemporary in-Kernel layer 3 VPN protocol implementation
which aims to provide fast and secure tunnels. The wireguard-quick executor
allows to set up Wireguard VPN tunnels using wg-quick.

The configuration file used by wg-quick will be named `/etc/wireguard/_interface_.conf`.

# WIREGUARD-RELATED OPTIONS

*wg-endpoint-resolution-retries* _integer_
	Denotes the number of times DNS resolution for each peer's
	endpoint will be retried for, with an increasing delay.
	Can be set to _infinity_. If unset, the default is _15_.


# EXAMPLES

A Wireguard VPN tunnel configured with wg-quick:

```
auto wg-foo
iface wg-foo
        use wireguard-quick
```

# AUTHORS

Ariadne Conill <ariadne@dereferenced.org>
