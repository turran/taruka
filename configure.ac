
m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [2])
m4_define([v_ver], [v_maj.v_min.v_mic])

m4_define([lt_cur], m4_eval(v_maj + v_min))
m4_define([lt_rev], v_mic)
m4_define([lt_age], v_min)

AC_INIT([taruka], [v_ver], [enesim-devel@googlegroups.com])
AC_PREREQ([2.60])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

## Add parameters for aclocal (keep in sync with Makefile.am)
AC_SUBST(ACLOCAL_AMFLAGS, "-I m4 -I common/m4")

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

### Needed information

AC_CANONICAL_HOST

requirements_taruka=""

efl_version="1.7.0"
efl_egueb_version="0.0.1"
egueb_version="0.0.23"
eon_version="0.0.15"

### Additional options to configure


### Checks for programs

AC_PROG_CC

# pkg-config
PKG_PROG_PKG_CONFIG

### Checks for libraries

case "$host_os" in
   mingw*)
      requirements_taruka="${requirements_taruka} evil >= ${efl_version}"
   ;;
esac

requirements_taruka="eon >= ${eon_version} efl-egueb >= ${efl_egueb_version} egueb-dom >= ${egueb_version} egueb-svg >= ${egueb_version} ecore >= ${efl_version} ${requirements_taruka}"
AC_SUBST([requirements_taruka])
PKG_CHECK_MODULES([TARUKA], [${requirements_taruka}])

### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics

AM_PROG_CC_C_O

EFL_CHECK_COMPILER_FLAGS([TARUKA], [-Wall -Wextra -Wshadow])


### Checks for linker characteristics

### Checks for library functions

AC_CONFIG_FILES([
Makefile
])

AC_OUTPUT

#####################################################################
## Info

echo
echo
echo
echo "------------------------------------------------------------------------"
echo "$PACKAGE $VERSION"
echo "------------------------------------------------------------------------"
echo
echo
echo "Configuration Options Summary:"
echo
echo "Compilation.................................: make (or gmake)"
echo "  CPPFLAGS..................................: $CPPFLAGS"
echo "  CFLAGS....................................: $CFLAGS"
echo "  LDFLAGS...................................: $LDFLAGS"
echo
echo "Installation................................: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix....................................: $prefix"
echo
