
bin_PROGRAMS = src/bin/trantor

src_bin_trantor_CPPFLAGS = \
-I$(top_srcdir)/src/bin \
@TRANTOR_CFLAGS@

src_bin_trantor_SOURCES = \
src/bin/trantor_main.c \
src/bin/trantor_view_xml.c

src_bin_trantor_LDADD = @TRANTOR_LIBS@
