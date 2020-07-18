# ifupdown-ng

This package is a work in progress implementation of the ifupdown suite.  It is
intended to be largely compatible with ifupdown and ifupdown2, with some caveats:

* ifupdown2 python plugins are not supported at this time.  An executor could be
  written to handle them.

* ifupdown-ng uses a SAT solver to determine interface bring-up order, like
  ifupdown2.  However, relationships must be explicitly defined instead of
  inferred by plugins in ifupdown2.  This simplifies the executors and ensures
  consistent behaviour across executors.

* ifupdown-ng retains compatibility with /etc/network/if-X.d scripts, but will
  prefer using executors in /usr/libexec/ifupdown-ng where appropriate.

This package is planned to replace BusyBox ifupdown in Alpine at some point in
the future.
