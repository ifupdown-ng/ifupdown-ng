interfaces-wifi(5)

# NAME

*interfaces-wifi* - WiFi vocabulary for the interfaces(5) file format

# DESCRIPTION

Wi-Fi (the IEEE 802.11 family of protocols) is a commonly used wireless
networking standard.  The following options allow for configuration of
Wi-Fi client interfaces.

# WIFI-RELATED OPTIONS

*wifi-config-path* _path_
	Denotes the absolute _path_ to a *wpa_supplicant* configuration file.
	If no path is given, _/run/wpa_supplicant.<interface>.conf_ will be
	used for a temporary configuration file.  This option may not be used
	with other configuration options.

*wifi-ssid* _ssid_
	The SSID the Wi-Fi client should connect to.

*wifi-psk* _psk_
	The passphrase for connecting to the Wi-Fi network.  If unset, the
	client will connect without WPA2 encryption.

# EXAMPLES

A typical setup may involve connecting to a home and work network.  To
achieve this, we can define a pair of virtual interfaces called *wifi-home*
and *wifi-work*, which connect to their respective wifi networks:

```
iface wifi-home
	use dhcp
	wifi-ssid HomeNetwork
	wifi-psk ExamplePassphrase

iface wifi-work
	use dhcp
	wifi-config-path /etc/network/wpa-work.conf
```

The virtual interfaces can be used with *ifup* and *ifdown*:

```
# ifup wlan0=wifi-home
# ifdown wlan0
# ifup wlan0=wifi-work
```

# AUTHORS

Ariadne Conill <ariadne@dereferenced.org>
