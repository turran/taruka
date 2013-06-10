MAINTAINERCLEANFILES = \
Makefile.in

AM_CPPFLAGS = \
-I$(top_builddir) \
-I$(top_srcdir)/src/bin \
@TRANTOR_CFLAGS@

bin_PROGRAMS = trantor

trantor_SOURCES = \
trantor_main.c \
trantor_view_xml.c
 
trantor_LDADD = @TRANTOR_LIBS@
