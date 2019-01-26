
#Common
AM_CFLAGS = -I$(top_srcdir)
LDADD = ../mdsl/libmdsl.la -lm

#Log compiler
nodist_check_SCRIPTS = logcc.sh
DISTCLEANFILES = logcc.sh
EXTRA_DIST = logcc.sh.in

#Unit tests
check_PROGRAMS = \
	 arrays \
	 private \
	 dict

TESTS = $(check_PROGRAMS)
LOG_COMPILER = sh $(builddir)/logcc.sh

