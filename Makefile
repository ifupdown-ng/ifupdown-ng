SCDOC := scdoc

PACKAGE_NAME := ifupdown-ng
PACKAGE_VERSION := 0.2.1
PACKAGE_BUGREPORT := https://github.com/kaniini/ifupdown-ng/issues/new


INTERFACES_FILE := /etc/network/interfaces
STATE_FILE := /run/ifstate
CFLAGS = -ggdb3 -Os -Wall -Wextra
CPPFLAGS = -I. -DINTERFACES_FILE=\"${INTERFACES_FILE}\" -DSTATE_FILE=\"${STATE_FILE}\" -DPACKAGE_NAME=\"${PACKAGE_NAME}\" -DPACKAGE_VERSION=\"${PACKAGE_VERSION}\" -DPACKAGE_BUGREPORT=\"${PACKAGE_BUGREPORT}\"


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
	libifupdown/lifecycle.c

LIBIFUPDOWN_OBJ = ${LIBIFUPDOWN_SRC:.c=.o}
LIBIFUPDOWN_LIB = libifupdown.a

MULTICALL_SRC = cmd/multicall.c
MULTICALL_OBJ = ${MULTICALL_SRC:.c=.o}
MULTICALL = ifupdown

IFQUERY_SRC = cmd/ifquery.c
IFQUERY_OBJ = ${IFQUERY_SRC:.c=.o}

IFUPDOWN_SRC = cmd/ifupdown.c
IFUPDOWN_OBJ = ${IFUPDOWN_SRC:.c=.o}

CMD_OBJ = ${MULTICALL_OBJ} ${IFQUERY_OBJ} ${IFUPDOWN_OBJ}

CMDS = ifup ifdown ifquery

LIBS = ${LIBIFUPDOWN_LIB}

all: libifupdown.a ${MULTICALL} ${CMDS}

${CMDS}: ${MULTICALL}
	ln -s ifupdown $@

${MULTICALL}: ${LIBS} ${CMD_OBJ}
	${CC} -o $@ ${CMD_OBJ} ${LIBS}

libifupdown.a: ${LIBIFUPDOWN_OBJ}
	${AR} -rcs $@ ${LIBIFUPDOWN_OBJ}

clean:
	rm -f ${LIBIFUPDOWN_OBJ} ${CMD_OBJ}
	rm -f ${CMDS} ${MULTICALL}
	rm -f ${MANPAGES}

check: libifupdown.a ${CMDS}
	kyua test

install: all
	install -D -m755 ${MULTICALL} ${DESTDIR}/sbin/${MULTICALL}
	for i in ${CMDS}; do \
		ln -s /sbin/${MULTICALL} ${DESTDIR}/sbin/$$i; \
	done

.scd.1 .scd.2 .scd.3 .scd.4 .scd.5 .scd.6 .scd.7 .scd.8:
	${SCDOC} < $< > $@

MANPAGES_8 = \
	doc/ifquery.8 \
	doc/ifup.8 \
	doc/ifdown.8

MANPAGES_5 = \
	doc/interfaces.5

MANPAGES = ${MANPAGES_5} ${MANPAGES_8}

docs: ${MANPAGES}

install_docs: docs
	for i in ${MANPAGES_5}; do \
		target=$$(basename $$i); \
		install -D -m644 $$i ${DESTDIR}/usr/share/man/man5/$$target; \
	done
	for i in ${MANPAGES_8}; do \
		target=$$(basename $$i); \
		install -D -m644 $$i ${DESTDIR}/usr/share/man/man8/$$target; \
	done

.SUFFIXES: .scd .1 .2 .3 .4 .5 .6 .7 .8
