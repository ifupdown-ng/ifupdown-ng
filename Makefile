CFLAGS := -ggdb3 -O2 -Wall -I.


LIBIFUPDOWN_SRC = \
	libifupdown/list.c
LIBIFUPDOWN_OBJ = ${LIBIFUPDOWN_SRC:.c=.o}


libifupdown.a: ${LIBIFUPDOWN_OBJ}
	ar -rcs $@ ${LIBIFUPDOWN_OBJ}

all: libifupdown.a

clean:
	rm -f ${LIBIFUPDOWN_OBJ}
