LAYOUT ?= linux
SCDOC := scdoc
LIBBSD_CFLAGS =
LIBBSD_LIBS =

PACKAGE_NAME := ifupdown-ng
PACKAGE_VERSION := 0.12.1
PACKAGE_BUGREPORT := https://github.com/ifupdown-ng/ifupdown-ng/issues/new

SBINDIR := /sbin

ARTIFACTS_PATH ?= ./

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
CPPFLAGS += -DEXECUTOR_PATH=\"${EXECUTOR_ФPATH}\"


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
LIBIFUPDOWN_LIB = libifupdown.a

MULTICALL_SRC = \
	cmd/multicall.c \
	cmd/multicall-options.c \
	cmd/multicall-exec-options.c \
	cmd/multicall-match-options.c \
	cmd/pretty-print-iface.c
MULTICALL_OBJ = ${MULTICALL_SRC:.c=.o}
MULTICALL = ifupdown

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
CMDS += ${CMDS_Y}
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

TARGET_LIBS = $(addprefix ${ARTIFACTS_PATH},${LIBIFUPDOWN_LIB})
LIBS += ${TARGET_LIBS} ${LIBBSD_LIBS}

all: ${MULTICALL} ${CMDS}

${CMDS}: ${MULTICALL}
	ln -sf ifupdown $(addprefix ${ARTIFACTS_PATH},$@)

${MULTICALL}: ${TARGET_LIBS} ${MULTICALL_OBJ}
	${CC} ${LDFLAGS} -o ${ARTIFACTS_PATH}$@ ${MULTICALL_OBJ} ${LIBS}

${LIBIFUPDOWN_LIB}: ${LIBIFUPDOWN_OBJ}
	${AR} -rcs ${ARTIFACTS_PATH}$@ ${LIBIFUPDOWN_OBJ}

clean:
	rm -f ${LIBIFUPDOWN_OBJ} ${MULTICALL_OBJ}
	rm -f $(addprefix ${ARIFACTS_PATH},${LIBIFUPDOWN_LIB})
	rm -f $(addprefix ${ARTIFACTS_PATH},${CMDS}) ${ARTIFACTS_PATH}${MULTICALL}
	rm -f ${ARTIFACTS_PATH}${MANPAGES}

check: ${LIBIFUPDOWN_LIB} ${CMDS}
	kyua test || (kyua report --verbose && exit 1)

install: all
	install -D -m755 ${MULTICALL} ${DESTDIR}${SBINDIR}/${MULTICALL}
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

.scd.1 .scd.2 .scd.3 .scd.4 .scd.5 .scd.6 .scd.7 .scd.8:
	${SCDOC} < $< > ${ARTIFACTS_PATH}$@

MANPAGES_5 = \
	doc/ifstate.5 \
	doc/ifupdown-ng.conf.5 \
	doc/interfaces.5 \
	doc/interfaces-bond.5 \
	doc/interfaces-batman.5 \
	doc/interfaces-bridge.5 \
	doc/interfaces-forward.5 \
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

MANPAGES = ${MANPAGES_5} ${MANPAGES_7} ${MANPAGES_8}

docs: ${MANPAGES}

install_docs: docs
	for i in ${MANPAGES_5}; do \
		target=$$(basename $$i); \
		install -D -m644 $$i ${DESTDIR}/usr/share/man/man5/$$target; \
	done
	for i in ${MANPAGES_7}; do \
		target=$$(basename $$i); \
		install -D -m644 $$i ${DESTDIR}/usr/share/man/man7/$$target; \
	done
	for i in ${MANPAGES_8}; do \
		target=$$(basename $$i); \
		install -D -m644 $$i ${DESTDIR}/usr/share/man/man8/$$target; \
	done

.SUFFIXES: .scd .1 .2 .3 .4 .5 .6 .7 .8

DIST_NAME = ${PACKAGE_NAME}-${PACKAGE_VERSION}
DIST_TARBALL = ${DIST_NAME}.tar.xz

distcheck: check dist
dist: ${DIST_TARBALL}
${DIST_TARBALL}:
	git archive --format=tar --prefix=${DIST_NAME}/ -o ${DIST_NAME}.tar ${DIST_NAME}
	xz ${DIST_NAME}.tar
