CFLAGS := -ggdb3 -O2 -Wall -I.


LIBIFUPDOWN_SRC = \
	libifupdown/list.c \
	libifupdown/dict.c \
	libifupdown/interface.c

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
	ar -rcs $@ ${LIBIFUPDOWN_OBJ}

all: libifupdown.a ${CMDS}

clean:
	rm -f ${LIBIFUPDOWN_OBJ}
