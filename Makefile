INTERFACES_FILE := /etc/network/interfaces
CFLAGS := -ggdb3 -O2 -Wall -I. -DINTERFACES_FILE=\"${INTERFACES_FILE}\"


LIBIFUPDOWN_SRC = \
	libifupdown/list.c \
	libifupdown/dict.c \
	libifupdown/interface.c \
	libifupdown/interface-file.c \
	libifupdown/fgetline.c

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
	rm -f ${LIBIFUPDOWN_OBJ}
