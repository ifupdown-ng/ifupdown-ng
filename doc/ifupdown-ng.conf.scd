ifupdown-ng.conf(5)

# NAME

*ifupdown-ng.conf* - Global configuration file for ifupdown-ng

# DESCRIPTION

ifupdown-ng allows to configure some parts of it's behaviour via global
configuration options.

# GENERAL CONFIGURATION OPTIONS

*allow_addon_scripts* _bool_
	Enable support for /etc/network/if-X.d addon scripts.  These are used for
	compatibility with legacy setups, and may be disabled for
	performance improvements in setups where only ifupdown-ng executors
	are used.  Valid values are _0_ and _1_, the default is _1_.

*auto_executor_selection* _bool_
	Automatically determine which executors to use.  At present, this
	is done by inserting `use` statements for the namespace a config
	option has.  The namespace is separated from the config option with
	a dash (`-`).  Valid values are _0_ and _1_, the default is _1_.

*use_hostname_for_dhcp* _bool_
	Automatically learn the hostname property, used for DHCP
	configuration by querying the system hostname using uname(2).
	This is basically equivalent to `hostname $(hostname)` without
	having to specify any configuration.  Valid values are _0_ and
	_1_, the default is _1_.

# TEMPLATE RELATED OPTIONS

*allow_any_iface_as_template* _bool_
	Enable any interface to act as a template for another interface.
	This is presently the default, but is deprecated.  An admin may
	choose to disable this setting in order to require inheritance
	from specified templates.  Valid values are _0_ and _1_, the
	default is _1_.

*implicit_template_conversion* _bool_
	In some legacy configs, a template may be declared as an iface, and
	ifupdown-ng automatically converts those declarations to a proper
	template.  If this setting is disabled, inheritance will continue
	to work against non-template interfaces without converting them to
	a template.  Valid values are _0_ and _1_, the default is _1_.

# COMPATIBILITY RELATED OPTIONS

*compat_create_interfaces* _bool_
	Denotes where or not to create interfaces when compat_\* settings are
	active and it would be necessary to create an interface to be fully
	compliant.  This could happen when inheriting bridge VLAN settings to
	an interface within a bridges bridge-ports setting but no interface
	stanza is found.  Valid values are _0_ and _1_, the default is _1_.

compat_ifupdown2_bridge_ports_inherit_vlans _bool_
	In ifupdown2 <bridge-vids> as well as the <bridge-pvid> set on a
	bridge interface will be inherited by all member ports if not set
	explicitly.  When set to 1 ifupdown-ng behaves the same way and will
	internally copy both options from the bridge member ports if they
	are not set on the member port.  Valid values are _0_ and _1_, the
	default is _1_.

# FILES

/etc/network/ifupdown-ng.conf

# SEE ALSO

*interfaces*(5)

# AUTHORS

Maximilian Wilhelm <max@sdn.clinic>
