# ifupdown-ng for system administrators

ifupdown-ng is a network device manager which is backwards
compatible with traditional ifup and ifdown as used on Debian
and Alpine systems, while solving many design deficits with
the original approach through robust error handling and the
use of a dependency-solver to determine interface bring-up
order.

This guide is intended to walk users through using the
ifupdown-ng system without any assumption of familiarity
with the legacy ifupdown system.

## Important Filesystem Paths

The ifupdown-ng system uses the following paths, ranked
in order of importance:

* `/etc/network/interfaces`: the interface configuration
  database, which contains information about what
  interfaces should be configured.

* `/etc/network/ifupdown-ng.conf`: the main configuration
  file which controls ifupdown-ng's behaviour.  See the
  *ifupdown-ng Configuration* section below.

* `/run/ifstate`: the interface state file, which denotes
  what physical interfaces are configured, and what
  interface definition they are configured as.

* `/usr/libexec/ifupdown-ng`: this directory contains the
  native ifupdown-ng executors, which are run as necessary
  to configure an interface.  See the ifupdown-executor(7)
  manual page for more information on how these programs
  are written.

* `/etc/network/if-{up|down|pre-up|post-down}.d`:
  these directories contain scripts that are run when an
  interface is brought up or down.  In general, they follow
  the same contract described in ifupdown-executor(7).

All configuration examples in this guide concern the
`/etc/network/interfaces` file.

## ifupdown-ng Configuration

ifupdown-ng allows to configure some parts of it's behaviour.
Currently the following settings are supported in
`/etc/network/ifupdown-ng.conf`:

* `allow_addon_scripts`: Enable support for /etc/if-X.d addon scripts.
  These are used for compatibility with legacy setups, and may be
  disabled for performance improvements in setups where only
  ifupdown-ng executors are used.  Valid values are `0` and `1`,
  default is `1`.

* `allow_any_iface_as_template`: Enable any interface to act as a
  template for another interface.  This is presently the default,
  but is deprecated.  An admin may choose to disable this setting
  in order to require inheritance from specified templates.
  Valid values are `0` and `1`, the default is `1`.

* `implicit_template_conversion`: In some legacy configs, a template
  may be declared as an iface, and ifupdown-ng automatically converts
  those declarations to a proper template.  If this setting is
  disabled, inheritance will continue to work against non-template
  interfaces without converting them to a template. Valid values
  are `0` and `1`, the default is `1`.

## Interface Configuration

### Basic Configuration

To begin with, lets look at a basic configuration for a
desktop computer.  This scenario involves using the DHCP
helper to learn an IPv4 address dynamically.

In this case, the `/etc/network/interfaces` file would
look like:

```
auto eth0
iface eth0
    use dhcp
```

These configuration statements do two things: designate
that `eth0` should be started automatically with the `auto`
keyword, and designate that the `dhcp` executor should be
used to configure the interface.

As a more detailed explanation, here is a commented version:

```
# Start eth0 automatically.
auto eth0

# Begin an interface definition for eth0.
iface eth0

    # Use the dhcp executor to configure eth0.
    use dhcp
```

### IPv6 RA Configuration

With IPv6, stateless auto-configuration is typically used to
configure network interfaces.  If you are not interested in
using IPv4 at all, you can simply use the `ipv6-ra` executor
to ensure that an interface is configured to accept IPv6 RA
advertisements:

```
auto eth0
iface eth0
    use ipv6-ra
```

### Static Configuration

We can use the `static` executor to configure static IPv4 and
IPv6 addresses.  If you use the `address` keyword, the `static`
executor will automatically be used to configure the interface:

```
auto eth0
iface eth0
    address 203.0.113.2/24
    gateway 203.0.113.1
```

#### Multiple Addresses

A typical scenario on servers is where a server has multiple
IP addresses on a single interface.  In this case you simply
add additional `address` lines like this:

```
auto eth0
iface eth0
    address 203.0.113.2/24
    address 203.0.113.3/24
    address 203.0.113.4/24
    gateway 203.0.113.1
```

#### Dual-stack configurations

Another typical scenario for servers is to run a dual-stack
configuration, where interfaces have both an IPv4 and an IPv6
address.  This is accomplished in a similar way as multi-homing.
You specify the IPv4 and IPv6 addresses you want, followed by
gateways for each:

```
auto eth0
iface eth0
    address 203.0.113.2/24
    address 203.0.113.3/24
    address 203.0.113.4/24
    gateway 203.0.113.1

    address 2001:db8:1000:2::2/64
    address 2001:db8:1000:2::3/64
    address 2001:db8:1000:2::4/64
    gateway 2001:db8:1000:2::1
```

## Relationships

As previously mentioned, ifupdown-ng features a dependency
resolver that allows for determining the interface configuration
order.

![Dependency resolution example](img/dependency-resolution.png)

In order to make use of this, dependencies can be managed in one
of two ways:

### Explicit dependency management using `requires`

The `requires` keyword can be used to manage explicit
dependencies:

```
auto eth0
iface eth0
    use dhcp

auto gre0
iface gre0
    requires eth0

    use gre
    gre-endpoint 203.0.113.2
    gre-ttl 255
    gre-flags ignore-df

    address 203.0.113.194/30
    gateway 203.0.113.193
```

### Implicit dependency management using executors

Executors can declare implicit dependencies which work the same
way as explicit dependencies, but are learned at run-time, for
example:

```
auto bond0
iface bond0
    use bond

    bond-members eth0 eth1
    [...]
```

Is with respect to dependency equivalent to:

```
auto bond0
iface bond0
    use bond

    requires eth0 eth1
    [...]
```

## Executors

The ifupdown-ng system is expanded with additional features via
executors.  Executors are selected on a per-interface basis using
`use` statements, for example:

```
auto eth0
iface eth0
    use dhcp
```

Executors are run in the order specified by the `use` statements.
Some executors are automatically added based on other statements
in an interface definition.  To see the full list of executors
used for an interface, use the ifquery(8) command.

## Questions

If you have further questions about how to use ifupdown-ng to
configure a specific scenario, drop by the
[ifupdown-ng IRC channel](irc://irc.as7007.net/#ifupdown-ng).
