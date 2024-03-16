LAYOUT ?= linux
SCDOC := scdoc
LIBBSD_CFLAGS =
LIBBSD_LIBS =

PACKAGE_NAME := ifupdown-ng
PACKAGE_VERSION := 0.12.1
PACKAGE_BUGREPORT := https://github.com/ifupdown-ng/ifupdown-ng/issues/new

SBINDIR := /sbin

ifdef BUILDDIR
	BUILDDIR_ := $(patsubst %/,%,${BUILDDIR})/
else
	BUILDDIR_ :=
endif

INTERFACES_FILE := /etc/network/interfaces
STATE_FILE := /run/ifstate
CONFIG_FILE := /etc/network/ifupdown-ng.conf
EXECUTOR_PATH := /usr/libexec/ifupdown-ng

CFLAGS ?= -ggdb3 -Os
CFLAGS += -Wall -Wextra -Werror
CFLAGS += -Wmissing-declarations -Wmissing-prototypes -Wcast-align -Wpointer-arith -Wreturn-type
CFLAGS += ${LIBBSD_CFLAGS}
CPPFLAGS = -I.
CPPFLAGS += -DINTERFACES_FILE=\"${INTERFACES_FILE}\"
CPPFLAGS += -DSTATE_FILE=\"${STATE_FILE}\"
CPPFLAGS += -DCONFIG_FILE=\"${CONFIG_FILE}\"
CPPFLAGS += -DPACKAGE_NAME=\"${PACKAGE_NAME}\"
CPPFLAGS += -DPACKAGE_VERSION=\"${PACKAGE_VERSION}\"
CPPFLAGS += -DPACKAGE_BUGREPORT=\"${PACKAGE_BUGREPORT}\"
CPPFLAGS += -DEXECUTOR_PATH=\"${EXECUTOR_PATH}\"


LIBIFUPDOWN_SRC = \
	libifupdown/list.c \
	libifupdown/dict.c \
	libifupdown/interface.c \
	libifupdown/interface-file.c \
	libifupdown/fgetline.c \
	libifupdown/version.c \
	libifupdown/state.c \
	libifupdown/environment.c \
	libifupdown/execute.c \
	libifupdown/lifecycle.c \
	libifupdown/config-parser.c \
	libifupdown/config-file.c \
	libifupdown/compat.c
LIBIFUPDOWN_OBJ = ${LIBIFUPDOWN_SRC:.c=.o}
LIBIFUPDOWN_OBJ_PREFIXED = $(addprefix ${BUILDDIR_},${LIBIFUPDOWN_OBJ})
LIBIFUPDOWN_LIB = libifupdown.a
LIBIFUPDOWN_LIB_PREFIXED = ${BUILDDIR_}${LIBIFUPDOWN_LIB}

MULTICALL_SRC = \
	cmd/multicall.c \
	cmd/multicall-options.c \
	cmd/multicall-exec-options.c \
	cmd/multicall-match-options.c \
	cmd/pretty-print-iface.c
MULTICALL_OBJ = ${MULTICALL_SRC:.c=.o}
MULTICALL = ifupdown
MULTICALL_PREFIXED = ${BUILDDIR_}${MULTICALL}

# enable ifup/ifdown applets (+16 KB)
CONFIG_IFUPDOWN ?= Y
IFUPDOWN_SRC = cmd/ifupdown.c
MULTICALL_${CONFIG_IFUPDOWN}_OBJ += ${IFUPDOWN_SRC:.c=.o}
CMDS_${CONFIG_IFUPDOWN} += ifup ifdown

# enable ifquery applet (+4 KB)
# [+20 KB without ifup/ifdown]
CONFIG_IFQUERY ?= Y
IFQUERY_SRC = cmd/ifquery.c
MULTICALL_${CONFIG_IFQUERY}_OBJ += ${IFQUERY_SRC:.c=.o}
CMDS_${CONFIG_IFQUERY} += ifquery

# enable ifctrstat applet (+1 KB)
CONFIG_IFCTRSTAT ?= Y
IFCTRSTAT_SRC = cmd/ifctrstat.c cmd/ifctrstat-${LAYOUT}.c
MULTICALL_${CONFIG_IFCTRSTAT}_OBJ += ${IFCTRSTAT_SRC:.c=.o}
CMDS_${CONFIG_IFCTRSTAT} += ifctrstat

# enable ifparse applet (+1 KB)
CONFIG_IFPARSE ?= Y
IFPARSE_SRC = cmd/ifparse.c
MULTICALL_${CONFIG_IFPARSE}_OBJ += ${IFPARSE_SRC:.c=.o}
CMDS_${CONFIG_IFPARSE} += ifparse

# enable YAML support (+2 KB)
CONFIG_YAML ?= Y
YAML_SRC = \
	libifupdown/yaml-base.c \
	libifupdown/yaml-writer.c
LIBIFUPDOWN_${CONFIG_YAML}_OBJ += ${YAML_SRC:.c=.o}
CPPFLAGS_${CONFIG_YAML} += -DCONFIG_YAML

LIBIFUPDOWN_OBJ += ${LIBIFUPDOWN_Y_OBJ}
MULTICALL_OBJ += ${MULTICALL_Y_OBJ}
MULTICALL_OBJ_PREFIXED = $(addprefix ${BUILDDIR_},${MULTICALL_OBJ})
CMDS += ${CMDS_Y}
CMDS_PREFIXED = $(addprefix ${BUILDDIR_},${CMDS})
CPPFLAGS += ${CPPFLAGS_Y}

EXECUTOR_SCRIPTS_CORE ?= \
	dhcp \
	ipv6-ra \
	static \
	link \
	ppp \
	forward

EXECUTOR_SCRIPTS_OPT ?= \
	batman \
	bond \
	bridge \
	cake \
	cake-ingress \
	ethtool \
	gre \
	mpls \
	tunnel \
	vrf \
	vxlan \
	wifi \
	wireguard \
	wireguard-quick

EXECUTOR_SCRIPTS ?= ${EXECUTOR_SCRIPTS_CORE} ${EXECUTOR_SCRIPTS_OPT}

EXECUTOR_SCRIPTS_STUB ?=

EXECUTOR_SCRIPTS_NATIVE ?=

TARGET_LIBS = ${LIBIFUPDOWN_LIB}
TARGET_LIBS_PREFIXED = $(addprefix ${BUILDDIR_},${TARGET_LIBS})
LIBS += ${TARGET_LIBS_PREFIXED} ${LIBBSD_LIBS}

all: ${MULTICALL_PREFIXED} ${CMDS_PREFIXED}

${CMDS_PREFIXED}: ${MULTICALL_PREFIXED}
	ln -sf ifupdown $@

${MULTICALL_PREFIXED}: ${TARGET_LIBS_PREFIXED} ${MULTICALL_OBJ_PREFIXED}
	${CC} ${LDFLAGS} -o $@ \
		${MULTICALL_OBJ_PREFIXED} ${LIBS}

${LIBIFUPDOWN_LIB_PREFIXED}: ${LIBIFUPDOWN_OBJ_PREFIXED}
	${AR} -rcs $@ ${LIBIFUPDOWN_OBJ_PREFIXED}

${BUILDDIR_}%.o: %.c
	${CC} ${CFLAGS} ${CPPFLAGS} -o $@ -c $<

clean:
	rm -f ${LIBIFUPDOWN_OBJ_PREFIXED} \
		${MULTICALL_OBJ_PREFIXED}
	rm -f ${LIBIFUPDOWN_LIB_PREFIXED}
	rm -f ${CMDS_PREFIXED} ${MULTICALL_PREFIXED}
	rm -f ${MANPAGES_PREFIXED}

check: ${LIBIFUPDOWN_LIB_PREFIXED} ${CMDS_PREFIXED}
	PATH=${BUILDDIR_}:$$PATH kyua test || (kyua report --verbose && exit 1)

install: all
	install -D -m755 ${MULTICALL_PREFIXED} ${DESTDIR}${SBINDIR}/${MULTICALL}
	for i in ${CMDS}; do \
		ln -s ${SBINDIR}/${MULTICALL} ${DESTDIR}${SBINDIR}/$$i; \
	done
	for i in ${EXECUTOR_SCRIPTS}; do \
		install -D -m755 executor-scripts/${LAYOUT}/$$i ${DESTDIR}${EXECUTOR_PATH}/$$i; \
	done
	for i in ${EXECUTOR_SCRIPTS_STUB}; do \
		install -D -m755 executor-scripts/stub/$$i ${DESTDIR}${EXECUTOR_PATH}/$$i; \
	done
	for i in ${EXECUTOR_SCRIPTS_NATIVE}; do \
		install -D -m755 executor-scripts/${LAYOUT}-native/$$i ${DESTDIR}${EXECUTOR_PATH}/$$i; \
	done
	install -D -m644 dist/ifupdown-ng.conf.example ${DESTDIR}${CONFIG_FILE}.example

${BUILDDIR_}%.5: %.scd
	mkdir -p ${BUILDDIR_}doc
	${SCDOC} < $< > $@

${BUILDDIR_}%.7: %.scd
	mkdir -p ${BUILDDIR_}doc
	${SCDOC} < $< > $@

${BUILDDIR_}%.8: %.scd
	mkdir -p ${BUILDDIR_}doc
	${SCDOC} < $< > $@

MANPAGES_5 = \
	doc/ifstate.5 \
	doc/ifupdown-ng.conf.5 \
	doc/interfaces.5 \
	doc/interfaces-bond.5 \
	doc/interfaces-batman.5 \
	doc/interfaces-bridge.5 \
	doc/interfaces-forward.5 \
	doc/interfaces-dhcp.5 \
	doc/interfaces-cake.5 \
	doc/interfaces-cake-ingress.5 \
	doc/interfaces-ppp.5 \
	doc/interfaces-tunnel.5 \
	doc/interfaces-vrf.5 \
	doc/interfaces-vxlan.5 \
	doc/interfaces-wifi.5 \
	doc/interfaces-wireguard.5 \
	doc/interfaces-wireguard-quick.5

MANPAGES_7 = \
	doc/ifupdown-executor.7

MANPAGES_8 = \
	doc/ifquery.8 \
	doc/ifup.8 \
	doc/ifdown.8 \
	doc/ifctrstat.8 \
	doc/ifparse.8

MANPAGES_5_PREFIXED = $(addprefix ${BUILDDIR_},${MANPAGES_5})
MANPAGES_7_PREFIXED = $(addprefix ${BUILDDIR_},${MANPAGES_7})
MANPAGES_8_PREFIXED = $(addprefix ${BUILDDIR_},${MANPAGES_8})

MANPAGES = ${MANPAGES_5} ${MANPAGES_7} ${MANPAGES_8}
MANPAGES_PREFIXED = $(addprefix ${BUILDDIR_},${MANPAGES})

docs: ${MANPAGES_PREFIXED}

install_docs: docs
	for i in ${MANPAGES_5_PREFIXED}; do \
		target=$$(basename $$i); \
		install -D -m644 $$i ${DESTDIR}/usr/share/man/man5/$$target; \
	done
	for i in ${MANPAGES_7_PREFIXED}; do \
		target=$$(basename $$i); \
		install -D -m644 $$i ${DESTDIR}/usr/share/man/man7/$$target; \
	done
	for i in ${MANPAGES_8_PREFIXED}; do \
		target=$$(basename $$i); \
		install -D -m644 $$i ${DESTDIR}/usr/share/man/man8/$$target; \
	done

DIST_NAME = ${PACKAGE_NAME}-${PACKAGE_VERSION}
DIST_TARBALL = ${DIST_NAME}.tar.xz

distcheck: check dist
dist: ${DIST_TARBALL}
${DIST_TARBALL}:
	git archive --format=tar --prefix=${DIST_NAME}/ -o ${DIST_NAME}.tar ${DIST_NAME}
	xz ${DIST_NAME}.tar
