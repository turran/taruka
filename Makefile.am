AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4 -I common/m4

MAINTAINERCLEANFILES = \
Makefile.in \
aclocal.m4 \
autom4te.cache \
config.guess \
config.sub \
configure \
depcomp \
install-sh \
ltmain.sh \
missing

### The app itself

include src/bin/Makefile.mk

### Data

include data/Makefile.mk

EXTRA_DIST = \
AUTHORS \
COPYING \
README \
autogen.sh \
m4/efl_compiler.m4
