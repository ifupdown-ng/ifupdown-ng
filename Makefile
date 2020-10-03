LAYOUT ?= linux
SCDOC := scdoc
LIBBSD_CFLAGS =
LIBBSD_LIBS =

PACKAGE_NAME := ifupdown-ng
PACKAGE_VERSION := 0.9.0
PACKAGE_BUGREPORT := https://github.com/ifupdown-ng/ifupdown-ng/issues/new


INTERFACES_FILE := /etc/network/interfaces
STATE_FILE := /run/ifstate
CONFIG_FILE := /etc/network/ifupdown-ng.conf
EXECUTOR_PATH := /usr/libexec/ifupdown-ng

CFLAGS ?= -ggdb3 -Os
CFLAGS += -Wall -Wextra
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
	libifupdown/config-file.c

LIBIFUPDOWN_OBJ = ${LIBIFUPDOWN_SRC:.c=.o}
LIBIFUPDOWN_LIB = libifupdown.a

MULTICALL_SRC = \
	cmd/multicall.c \
	cmd/multicall-options.c \
	cmd/multicall-exec-options.c \
	cmd/multicall-match-options.c
MULTICALL_OBJ = ${MULTICALL_SRC:.c=.o}
MULTICALL = ifupdown

# enable ifup/ifdown applets (+16 KB)
CONFIG_IFUPDOWN ?= Y
IFUPDOWN_SRC = cmd/ifupdown.c
MULTICALL_${CONFIG_IFUPDOWN}_OBJ += ${IFUPDOWN_SRC:.c=.o}
CMDS_${CONFIG_IFUPDOWN} += ifup ifdown
CPPFLAGS_${CONFIG_IFUPDOWN} += -DCONFIG_IFUPDOWN

# enable ifquery applet (+4 KB)
# [+20 KB without ifup/ifdown]
CONFIG_IFQUERY ?= Y
IFQUERY_SRC = cmd/ifquery.c
MULTICALL_${CONFIG_IFQUERY}_OBJ += ${IFQUERY_SRC:.c=.o}
CMDS_${CONFIG_IFQUERY} += ifquery
CPPFLAGS_${CONFIG_IFQUERY} += -DCONFIG_IFQUERY

# enable ifctrstat applet (+1 KB)
CONFIG_IFCTRSTAT ?= Y
IFCTRSTAT_SRC = cmd/ifctrstat.c cmd/ifctrstat-${LAYOUT}.c
MULTICALL_${CONFIG_IFCTRSTAT}_OBJ += ${IFCTRSTAT_SRC:.c=.o}
CMDS_${CONFIG_IFCTRSTAT} += ifctrstat
CPPFLAGS_${CONFIG_IFCTRSTAT} += -DCONFIG_IFCTRSTAT

MULTICALL_OBJ += ${MULTICALL_Y_OBJ}
CMDS += ${CMDS_Y}
CPPFLAGS += ${CPPFLAGS_Y}

EXECUTOR_SCRIPTS_CORE ?= \
	dhcp \
	ipv6-ra \
	static \
	link \
	ppp

EXECUTOR_SCRIPTS_OPT ?= \
	bridge \
	vrf \
	tunnel \
	gre \
	wireguard \
	ethtool \
	batman

EXECUTOR_SCRIPTS ?= ${EXECUTOR_SCRIPTS_CORE} ${EXECUTOR_SCRIPTS_OPT}

EXECUTOR_SCRIPTS_STUB ?=

TARGET_LIBS = ${LIBIFUPDOWN_LIB}
LIBS += ${TARGET_LIBS} ${LIBBSD_LIBS}

all: ${MULTICALL} ${CMDS}

${CMDS}: ${MULTICALL}
	ln -sf ifupdown $@

${MULTICALL}: ${TARGET_LIBS} ${MULTICALL_OBJ}
	${CC} -o $@ ${MULTICALL_OBJ} ${LIBS}

${LIBIFUPDOWN_LIB}: ${LIBIFUPDOWN_OBJ}
	${AR} -rcs $@ ${LIBIFUPDOWN_OBJ}

clean:
	rm -f ${LIBIFUPDOWN_OBJ} ${MULTICALL_OBJ}
	rm -f ${LIBIFUPDOWN_LIB}
	rm -f ${CMDS} ${MULTICALL}
	rm -f ${MANPAGES}

check: ${LIBIFUPDOWN_LIB} ${CMDS}
	kyua test || (kyua report --verbose && exit 1)

install: all
	install -D -m755 ${MULTICALL} ${DESTDIR}/sbin/${MULTICALL}
	for i in ${CMDS}; do \
		ln -s /sbin/${MULTICALL} ${DESTDIR}/sbin/$$i; \
	done
	for i in ${EXECUTOR_SCRIPTS}; do \
		install -D -m755 executor-scripts/${LAYOUT}/$$i ${DESTDIR}${EXECUTOR_PATH}/$$i; \
	done
	for i in ${EXECUTOR_SCRIPTS_STUB}; do \
		install -D -m755 executor-scripts/stub/$$i ${DESTDIR}${EXECUTOR_PATH}/$$i; \
	done
	install -D -m644 dist/ifupdown-ng.conf.example ${DESTDIR}${CONFIG_FILE}.example

.scd.1 .scd.2 .scd.3 .scd.4 .scd.5 .scd.6 .scd.7 .scd.8:
	${SCDOC} < $< > $@

MANPAGES_8 = \
	doc/ifquery.8 \
	doc/ifup.8 \
	doc/ifdown.8 \
	doc/ifctrstat.8

MANPAGES_5 = \
	doc/interfaces.5 \
	doc/interfaces-bond.5 \
	doc/interfaces-batman.5 \
	doc/interfaces-bridge.5 \
	doc/interfaces-vrf.5 \
	doc/interfaces-vxlan.5 \
	doc/interfaces-wireguard.5

MANPAGES_7 = \
	doc/ifupdown-executor.7

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
