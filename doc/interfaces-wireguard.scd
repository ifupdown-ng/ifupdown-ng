interfaces-wireguard(5)

# NAME

*interfaces-wireguard* - Wireguard extensions for the interfaces(5) file format

# DESCRIPTION

Wireguard is a comtemporary in-Kernel layer 3 VPN protocol implementation
which aims to provide fast and secure tunnels. The following options
allow to set up Wireguard VPN tunnels.

# WIREGUARD-RELATED OPTIONS

*wireguard-config-path* _path_
	Denotes the absolute _path_ to the Wireguard configuration file.
	If no path is given, _/etc/wireguard/<interface>.conf_ will be
	used. In the latter case _use wireguard_ has to be explicitly
	set to the interface configuration.

	Be aware that the given configuration file will be loaded using
	*wg setconf* and not with *wg-quick*.  The file format for both
	tools isn't compatible so you have to make sure you provide a
	valid configuration file for the *wg* tool.  If you already have
	a configuration file for *wg-quick* you can set up the tunnel
	manually once and then dump the configuration using *wg showconf*
	and save this to _path_.

*wireguard-quick*
	Use this executor for rely on *wg-quick* instead and use working
	*wg-quick* configurations.


# EXAMPLES

A Wireguard VPN tunnel with explicit configuration file specified

```
auto wg-foo
iface wg-foo
	wireguard-config-path /etc/wireguard/foo.conf
	#
	address 192.0.2.23/42
	address 2001:db8::23/64
```

A Wireguard VPN tunnel with implicit configuration file:

```
auto wg-bar
iface wg-bar
	use wireguard
	#
	address 192.0.2.23/42
	address 2001:db8::23/64
```

A Wireguard VPN tunnel with wg-quick:

```
auto wg-bar
iface wg-bar
	use wireguard-quick
```

# AUTHORS

Maximilian Wilhelm <max@sdn.clinic>
