PACKAGE_NAME := ifupdown-ng
PACKAGE_VERSION := 0.0.1
PACKAGE_BUGREPORT := https://github.com/kaniini/ifupdown-ng/issues/new


INTERFACES_FILE := /etc/network/interfaces
STATE_FILE := /run/ifstate
CFLAGS := -ggdb3 -O2 -Wall -I. -DINTERFACES_FILE=\"${INTERFACES_FILE}\" -DSTATE_FILE=\"${STATE_FILE}\" -DPACKAGE_NAME=\"${PACKAGE_NAME}\" -DPACKAGE_VERSION=\"${PACKAGE_VERSION}\" -DPACKAGE_BUGREPORT=\"${PACKAGE_BUGREPORT}\"


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


CMDS = ifquery ifup ifdown

LIBS = ${LIBIFUPDOWN_LIB}

all: libifupdown.a ${CMDS}

IFQUERY_SRC = cmd/ifquery.c
IFQUERY_OBJ = ${IFQUERY_SRC:.c=.o}
ifquery: ${LIBS} ${IFQUERY_OBJ}
	${CC} -o $@ ${IFQUERY_OBJ} ${LIBS}

IFUPDOWN_SRC = cmd/ifupdown.c
IFUPDOWN_OBJ = ${IFUPDOWN_SRC:.c=.o}
ifup: ${LIBS} ${IFUPDOWN_OBJ}
	${CC} -o $@ ${IFUPDOWN_OBJ} ${LIBS}

ifdown: ifup
	ln -s ifup $@

libifupdown.a: ${LIBIFUPDOWN_OBJ}
	${AR} -rcs $@ ${LIBIFUPDOWN_OBJ}

clean:
	rm -f ${LIBIFUPDOWN_OBJ} ${IFQUERY_OBJ} ${IFUPDOWN_OBJ}
	rm -f ${CMDS}

check: libifupdown.a ${CMDS}
	kyua test

install: all
	install -D -m755 ./ifquery ${DESTDIR}/sbin/ifquery
	install -D -m755 ./ifup ${DESTDIR}/sbin/ifup
	ln -s /sbin/ifup ${DESTDIR}/sbin/ifdown
