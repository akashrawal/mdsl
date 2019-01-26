
lib_LTLIBRARIES = libmdsl.la


#libmdsl.la
mdsl_c = \
	private.h \
	utils.c \
	arrays.c \
	dict.c

mdsl_h = mdsl.h incl.h \
	utils.h \
	arrays.h \
	dict.h
     
libmdsl_la_SOURCES = $(mdsl_c) $(mdsl_h)       
libmdsl_la_CFLAGS = -Wall -I$(top_builddir) -I$(top_srcdir)
#libmdsl_la_LDFLAGS = -lm

#Headers
mdslincludedir = $(includedir)/mdsl
mdslinclude_HEADERS = $(mdsl_h)                
