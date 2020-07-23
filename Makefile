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
	libifupdown/environment.c

LIBIFUPDOWN_OBJ = ${LIBIFUPDOWN_SRC:.c=.o}
LIBIFUPDOWN_LIB = libifupdown.a


CMDS = \
	ifquery

LIBS = ${LIBIFUPDOWN_LIB}

IFQUERY_SRC = cmd/ifquery.c
IFQUERY_OBJ = ${IFQUERY_SRC:.c=.o}
ifquery: ${LIBS} ${IFQUERY_OBJ}
	${CC} -o $@ ${IFQUERY_OBJ} ${LIBS}

libifupdown.a: ${LIBIFUPDOWN_OBJ}
	${AR} -rcs $@ ${LIBIFUPDOWN_OBJ}

all: libifupdown.a ${CMDS}

clean:
	rm -f ${LIBIFUPDOWN_OBJ} ${IFQUERY_OBJ}

check: libifupdown.a ${CMDS}
	kyua test
