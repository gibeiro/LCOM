COMPILER_TYPE= gnu

CC=gcc

PROG=	../bin/2048
SRCS=	main.c timer.c keyboard.c graphics.c vbe.c 2048.c bmp.c mouse.c

CCFLAGS= -Wall

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+=	-llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>

clean:
	rm -rf *o *d ${PROG}