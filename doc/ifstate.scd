ifstate(5)

# NAME

*/run/ifstate* - interface state database

# DESCRIPTION

The */run/ifstate* file describes the present state of the interface
configuration -- namely, how many interfaces are up, how many
dependencies each interface has which are up (the refcount), and
whether or not an interface was explicitly brought up due to request
or configuration.

# FILE SYNTAX

At a minimum, the */run/ifstate* file contains at least one column,
the physical to logical interface mapping.  This column is formatted
as such:

```
lo=lo
eth0=eth0
wlan0=work
```

The left side of the mapping is the physical interface, while the right
side is the logical interface.  This field is required to be present.

The next field is the reference count.  This is a number that reflects
the number of *active* dependencies an interface has.  As interfaces
are brought up and down, the refcount may change.  This field is
optional.

The final field denotes whether or not an interface was brought up
explicitly -- either by being marked as _auto_ or brought up manually
using *ifup*(8).  The contents of this field if present is the
_explicit_ keyword.

# EXAMPLES

An example from a typical system with localhost, eth0 and a wireguard
VPN:

```
lo=lo 1 explicit
eth0=eth0 2 explicit
wg0=wg0 1 explicit
```

# SEE ALSO

*interfaces*(5)

# AUTHORS

Ariadne Conill <ariadne@dereferenced.org>
