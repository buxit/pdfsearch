PROG	=	pdfsearch
CFLAGS	+=	-Os `pkg-config --cflags poppler-glib cairo`
LDFLAGS	+=	`pkg-config --libs poppler-glib cairo`
PREFIX	?=	/usr
MANDIR	?=	/usr/share/man

${PROG}: ${PROG}.c
	@gcc -o ${PROG} ${PROG}.c ${CFLAGS} ${LDFLAGS}

install: ${PROG}
	@install -Dm755 pdfsearch ${DESTDIR}${PREFIX}/bin/pdfsearch
	# @install -Dm644 pdfsearch.1 ${DESTDIR}${MANDIR}/man1/pdfsearch.1

clean:
	@rm -f pdfsearch
