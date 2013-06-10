
m4_define([v_maj], [0])
m4_define([v_min], [0])
m4_define([v_mic], [2])
m4_define([v_ver], [v_maj.v_min.v_mic])

m4_define([lt_cur], m4_eval(v_maj + v_min))
m4_define([lt_rev], v_mic)
m4_define([lt_age], v_min)

AC_INIT([trantor], [v_ver], [enesim-devel@googlegroups.com])
AC_PREREQ([2.60])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.6 dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

LT_PREREQ([2.2])
LT_INIT([win32-dll])

version_info="lt_cur:lt_rev:lt_age"
AC_SUBST([version_info])
VMAJ=v_maj
AC_SUBST([VMAJ])

### Needed information

AC_CANONICAL_HOST

### Additional options to configure


### Checks for programs

AC_PROG_CC

# pkg-config
PKG_PROG_PKG_CONFIG

### Checks for libraries

requirements_trantor="egueb-dom >= 0.0.21 egueb-svg >= 0.0.21 ender >= 0.0.6 enesim >= 0.0.15 ecore-evas >= 1.2.0 ecore >= 1.2.0 evas >= 1.2.0 eina >= 1.2.0 efl_svg"
AC_SUBST([requirements_trantor])
PKG_CHECK_MODULES([TRANTOR], [${requirements_trantor}])

### Checks for header files


### Checks for types


### Checks for structures


### Checks for compiler characteristics

EFL_CHECK_COMPILER_FLAGS([TRANTOR], [-Wall -Wextra -Wshadow])


### Checks for linker characteristics

### Checks for library functions

AC_CONFIG_FILES([
Makefile
src/Makefile
src/bin/Makefile
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
