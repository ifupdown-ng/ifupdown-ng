ifupdown-executor(7)

# NAME

*/usr/libexec/ifupdown-ng/program* - ifupdown executor protocol

# DESCRIPTION

The ifupdown executors are programs that are typically installed
into the ifupdown-ng executor path.  They follow a specific
protocol documented in this man page.

# PHASES

Executors are run to react to nine different phases and are not
required to take any specific action.  These phases are:

*depend*
	Called to determine if the executor wishes to change
	the dependency graph.  The executor should write a
	space-delimited list of interface names it is dependent
	upon to _stdout_.  Those interface names will be merged
	into the dependency graph.  If an executor does not have
	any dependencies, it may simply exit 0 without doing
	anything.

*create*
	Called before *pre-up*, to explicitly allow for interface
	creation if necessary.

*pre-up*
	Called before the interface is going to be brought up.

*up*
	Called when the interface is being brought up.

*post-up*
	Called after the interface was successfully brought up.

*pre-down*
	Called before the interface is going to be taken down.

*down*
	Called when the interface is being taken down.

*post-down*
	Called after the interface was successfully taken down.

*destroy*
	Called after *post-down* to allow for explicitly
	destroying an interface if necessary.

# ENVIRONMENT

Executors are guaranteed to run with a core set of environment
variables:

*IFACE*
	The name of the interface being configured.

*INTERFACES_FILE*
	The path to the interfaces database file being used.

*MODE*
	Either _start_, _stop_ or _depend_ depending on phase.
	This environment variable is present for compatibility
	with legacy ifupdown scripts and should not be used in
	ifupdown-ng executors.

*PHASE*
	The phase being executed.  See the phases section for
	more information about phases.

*VERBOSE*
	If present, verbose output is expected from the
	executor.

Additionally, the properties associated with an interface are
provided to executors.  The keys are rewritten to begin with
IF_ and are capitalized with dashes converted to underscores.
For example, the property _bridge-ports_ will be rewritten as
_IF_BRIDGE_PORTS_.

# SEE ALSO

ifup(8)++
ifdown(8)++
interfaces(5)

# AUTHORS

Ariadne Conill <ariadne@dereferenced.org>
