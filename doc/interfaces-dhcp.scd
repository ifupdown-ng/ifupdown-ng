interfaces-dhcp(5)

# NAME

*interfaces-dhcp* - DHCP extensions for the interfaces(5) file format

# DESCRIPTION

DHCP configuration is provided by one of the following clients, in
this order of preference: dhcpcd, dhclient, and udhcpc

No explict module configuration is required, but several options are
available:

# DHCP-RELATED OPTIONS

*dhcp-hostname* _name_
	Specified a hostname to be sent as part of DHCP queries. If
	not provided this value is guessed by ifupdown.
	This option is not supported for dhclient

*dhcp-vendor* _vendor_class_
	Specifies a vendor class to be sent as part of DHCP queries.
	This option is not supported for dhclient

*dhcp-client_id* _client_id_
	Specifies a client ID to be sent as part of DHCP queries.
	This option is not supported for dhclient

*dhcp-leasetime* _seconds_
	Requests a specific lease interval from the DHCP server.
	This option is not supported for dhclient

*dhcp-config* _path_
	Provide the specified config file to the DHCP client. This
	can be used to provide additional client configuration not
	available via these parameters.
	This option is not supported for udhcpc

*dhcp-script* _path_
	Specifies the script file called after DHCP events.

*dhcp-opts* _str_
	Additional arugments passed unmodified to the DHCP client.
	Use this to pass additional options otherwise not supported
	by this executor. For compatibility udhcpc also accepts this
	option when named: udhcpc-opts

# EXAMPLES

Typical (minimal) configuration

```
auto foo
iface foo
	use dhcp
```

All available options for a host using dhcpcd

```
auto foo
iface foo
	use dhcp
	dhcp-hostname bar
	dhcp-vendor MyVendor
	dhcp-client_id 0100BEEFC0FFEE
	dhcp-leasetime 86400
	dhcp-script /usr/lib/dhcpcd/dhcpcd-run-hooks
	dhcp-config /etc/dhcpcd-$IFACE.conf
	dhcp-opts --nogateway
```
