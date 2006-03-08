#----------------------------------------------------------------------------
# Autoconf input script. Invoke the ./autogen.sh script to generate a
# configure script from this file.
#----------------------------------------------------------------------------
AC_PREREQ([2.54])

#----------------------------------------------------------------------------
# Initialize Autoconf.
#----------------------------------------------------------------------------
AC_INIT(
    [peragro],
    [0.1],
    [info@peragro.nl])
CS_PACKAGEINFO(
    [Peragro Tempus],
    [Peragro Team],
    [peragro.nl])
AC_CONFIG_SRCDIR([mk/jam/build.jam])
AC_CONFIG_AUX_DIR([mk/autoconf])
AC_CANONICAL_HOST

#----------------------------------------------------------------------------
# Setup for the configuration header.
#----------------------------------------------------------------------------
AC_CONFIG_HEADERS([config.h])

#----------------------------------------------------------------------------
# Check for tools.
#----------------------------------------------------------------------------
CS_PROG_CC
AS_IF([test -z "$CC"],
    [AC_MSG_ERROR([Could not find a usable C compiler.])])
CS_PROG_CXX
AS_IF([test -z "$CXX"],
    [AC_MSG_ERROR([Could not find a usable C++ compiler.])])
CS_PROG_LINK

# The default RANLIB in Jambase is wrong on some platforms, and is also
# unsuitable during cross-compilation, so we set the value unconditionally
# (sixth argument of CS_EMIT_BUILD_PROPERTY).
AC_PROG_RANLIB
CS_EMIT_BUILD_PROPERTY([RANLIB], [$RANLIB], [], [], [], [Y])

CS_CHECK_TOOLS([WINDRES], [windres])
CS_EMIT_BUILD_PROPERTY([CMD.WINDRES], [$WINDRES])

CS_CHECK_PROGS([DOXYGEN], [doxygen])
CS_EMIT_BUILD_PROPERTY([CMD.DOXYGEN], [$DOXYGEN])

CS_CHECK_TOOLS([DOT], [dot])
CS_EMIT_BUILD_PROPERTY([CMD.DOT], [$DOT])

CS_CHECK_PROGS([TEXI2DVI], [texi2dvi])
CS_EMIT_BUILD_PROPERTY([CMD.TEXI2DVI], [$TEXI2DVI])

CS_CHECK_PROGS([TEXI2PDF], [texi2pdf])
CS_EMIT_BUILD_PROPERTY([CMD.TEXI2PDF], [$TEXI2PDF])

CS_CHECK_PROGS([DVIPS], [dvips])
CS_EMIT_BUILD_PROPERTY([CMD.DVIPS], [$DVIPS])

CS_CHECK_PROGS([DVIPDF], [dvipdf])
CS_EMIT_BUILD_PROPERTY([CMD.DVIPDF], [$DVIPDF])

CS_CHECK_PROGS([MAKEINFO], [makeinfo])
CS_EMIT_BUILD_PROPERTY([CMD.MAKEINFO], [$MAKEINFO])

CS_CHECK_PROGS([INSTALL], [install])
CS_EMIT_BUILD_PROPERTY([INSTALL], [$INSTALL])

CS_CHECK_LIBTOOL
CS_EMIT_BUILD_PROPERTY([LIBTOOL], [$LIBTOOL])
CS_EMIT_BUILD_PROPERTY([APPLE_LIBTOOL], [$APPLE_LIBTOOL])

CS_CHECK_MKDIR
CS_EMIT_BUILD_PROPERTY([CMD.MKDIR], [$MKDIR])
CS_EMIT_BUILD_PROPERTY([CMD.MKDIRS], [$MKDIRS])

CS_CHECK_PROGS([PERL], [perl5 perl])
CS_EMIT_BUILD_PROPERTY([PERL], [$PERL])

CS_CHECK_TEMPLATE_TOOLKIT2([emit])

CS_CHECK_CSWIN32LIBS

#----------------------------------------------------------------------------
# Determine system type
#----------------------------------------------------------------------------
CS_CHECK_HOST

#----------------------------------------------------------------------------
# Check for syntax problems / header files
#----------------------------------------------------------------------------
# Nothing yet.

#----------------------------------------------------------------------------
# Check for external libraries
#----------------------------------------------------------------------------
CS_CHECK_CPPUNIT([emit])

#----------------------------------------------------------------------------
# Check for CEGUI.
#----------------------------------------------------------------------------
CS_CHECK_LIB_WITH([CEGUI],
    [AC_LANG_PROGRAM([[#include <CEGUI.h>]],
    [CEGUI::WindowManager::getSingleton();])], [$srcdir/libs/csutil/win32/libs], [C++],
    [], [], [], [], [], [CEGUIBase])
AS_IF([test -z "$with_CEGUI"], [with_CEGUI=yes])

AS_IF([test $with_CEGUI != no],
[AS_IF([test $cs_cv_libCEGUI = yes],
    [CS_CHECK_BUILD([if CEGUI is sufficiently recent], [cs_cv_CEGUI_recent],
	[AC_LANG_PROGRAM(
	    [[#include <falagard/CEGUIFalWidgetLookManager.h>]],
	    [CEGUI::WidgetLookManager::getSingleton();])],
	[], [C++],
	[CS_EMIT_BUILD_RESULT([cs_cv_libCEGUI], [CEGUI])], [], [],
	[$cs_cv_libCEGUI_cflags],
	[$cs_cv_libCEGUI_lflags],
	[$cs_cv_libCEGUI_libs])], [AC_MSG_ERROR([
*** CEGUI could not be found. Version 0.4.0 is required.
*** You can get it from http://www.cegui.org.uk/
])])])

#----------------------------------------------------------------------------
# Check for Crystal Space (CS)
#----------------------------------------------------------------------------
AC_ARG_WITH([cs], [AC_HELP_STRING([--without-cs],
  [do not look for an installed Crystal Space SDK; this option is useful only
  if you need to perform routine maintenance tasks, such as generating
  project files or converting documentation, without having the Crystal Space
  SDK installed])])
AS_IF([test -z "$with_cs"], [with_cs=yes])

AS_IF([test $with_cs != no],
[CS_PATH_CRYSTAL_EMIT([0.99], [],
  [AC_MSG_ERROR([
*** Crystal Space could not be found or was unusable. The latest version is
*** always available from http://www.crystalspace3d.org/
*** Also, be sure that you have either installed Crystal Space or set the
*** CRYSTAL environment variable properly.
])], [crystalspace])])

#----------------------------------------------------------------------------
# Check for Crystal Entity Layer (CEL)
#----------------------------------------------------------------------------
m4_define([cel_desired], [yes])
CS_EMIT_BUILD_PROPERTY([CEL.DESIRED], [cel_desired])
m4_if(cel_desired, [yes],
[AC_ARG_WITH([cel], [AC_HELP_STRING([--without-cel],
  [do not look for an installed Crystal Entity Layer SDK; this option is useful
  only if you need to perform routine maintenance tasks, such as generating
  project files or converting documentation, without having the CEL SDK
  installed])])
AS_IF([test -z "$with_cel"], [with_cel=yes])

AS_IF([test $with_cel != no],
[CS_PATH_CEL_EMIT([0.99], [],
  [AC_MSG_ERROR([
*** CEL could not be found or was unusable. The latest version is always
*** available from http://cel.crystalspace3d.org/
])])
AC_DEFINE([USE_CEL], [], [Define to enable CEL.])
AC_DEFINE_UNQUOTED([CEL_PLUGIN_DIR], ["$CEL_PLUGIN_DIR"],
  [Define with path to CEL plugins.])
])])

#------------------------------------------------------------------------------
# Check for pthread.  Also check if pthread implementation supports the
# recursive mutex extension.
#------------------------------------------------------------------------------
CS_CHECK_PTHREAD([cygwin*])
CS_EMIT_BUILD_RESULT([cs_cv_sys_pthread], [PTHREAD])
AS_IF([test $cs_cv_sys_pthread_mutex_recursive != no],
    [CS_HEADER_PROPERTY([CS_PTHREAD_MUTEX_RECURSIVE],
	[$cs_cv_sys_pthread_mutex_recursive])])

#-----------------------------------------------------------------------------
# WinSock
#-----------------------------------------------------------------------------
case "$host_os" in
 *mingw*) 
 AC_CHECK_LIB(wsock32, main, [CS_EMIT_BUILD_PROPERTY([WSOCK32.LFLAGS], "-lwsock32")], AC_MSG_ERROR(Winsock is needed for a Win32 build.))
 ;;
esac

#----------------------------------------------------------------------------
# Package configuration switches.
#----------------------------------------------------------------------------
AC_ARG_ENABLE([debug],
    [AC_HELP_STRING([--enable-debug],
	[build with debugging information (default NO)])],
    [], [enable_debug=no])

AC_MSG_CHECKING([build mode])
AS_IF([test $enable_debug = yes], [build_mode=debug], [build_mode=optimize])
AC_MSG_RESULT([$build_mode])

CS_EMIT_BUILD_PROPERTY([MODE], [$build_mode])

#-----------------------------------------------------------------------------
# Emit install paths and package information.
#-----------------------------------------------------------------------------
CS_OUTPUT_INSTALLDIRS
CS_EMIT_PACKAGEINFO

#----------------------------------------------------------------------------
# Emit generated files.
#----------------------------------------------------------------------------
CS_JAMCONFIG_OUTPUT([Jamconfig])
AC_CONFIG_FILES([Jamfile])
AC_OUTPUT

AC_MSG_NOTICE([

Please note that this project uses Jam as its build tool,
therefore you must type 'jam' to build the project.

Jam home: http://www.perforce.com/jam/jam.html
Jam source: ftp://ftp.perforce.com/jam/
])
