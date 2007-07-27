# checkstackless.m4                                            -*- Autoconf -*-
#==============================================================================
# Based on checkpython.m4
# Copyright (C)2003,2004 by Eric Sunshine <sunshine@sunshineco.com>
#
#    This library is free software; you can redistribute it and/or modify it
#    under the terms of the GNU Library General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or (at your
#    option) any later version.
#
#    This library is distributed in the hope that it will be useful, but
#    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
#    License for more details.
#
#    You should have received a copy of the GNU Library General Public License
#    along with this library; if not, write to the Free Software Foundation,
#    Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
#==============================================================================
AC_PREREQ([2.56])

#------------------------------------------------------------------------------
# CS_CHECK_STACKLESS([EMITTER], [SDK-CHECK-DEFAULT], [WITH-DESCRIPTION])
#	Check for Stackless Python and a working Python SDK.  Sets the shell
#       variable STACKLESS to the name of the Python interpreter and invokes
#       AC_SUBST(). The shell variable cs_cv_stackless is set to "yes" if a
#       working Python SDK is discovered, else "no". If available, then the
#       variables cs_cv_stackless_cflags, cs_cv_stackless_lflags, and cs_cv_stackless_libs
#       are set. (As a convenience, these variables can be emitted to an output
#	file with CS_EMIT_BUILD_RESULT() by passing "cs_cv_stackless" as its
#	CACHE-VAR argument.) As a convenience, the shell variable
#	cs_cv_stackless_ext is set to the suffix of Python extension modules (with
#	leading dot; typically ".dll" or ".so").  The SDK check can be enabled
#	or disabled with --with[out]-stackless.  SDK-CHECK-DEFAULT should be
#	"with" or "without". If SDK-CHECK-DEFAULT is "with" or if it is
#	ommitted, then --with-stackless is the default, else --without-stackless is
#	the default.  WITH-DESCRIPTION is the description to use for the
#	--with[out]-stackless option. The literal string "use" (or "do not use")
#	is prepended to WITH-DESCRIPTION. If omitted, WITH-DESCRIPTION defaults
#	to "Stackless".  If EMITTER is provided, then CS_EMIT_BUILD_RESULT() is
#	invoked with EMITTER in order to record the results in an output
#	file. As a convenience, if EMITTER is the literal value "emit" or
#	"yes", then CS_EMIT_BUILD_RESULT()'s default emitter will be used.
#	When EMITTER is provided, the following properties are emitted to the
#	output file: STACKLESS (the actual interpreter), STACKLESS.AVAILABLE ("yes"
#	or "no"), STACKLESS.CFLAGS, STACKLESS.LFLAGS, and STACKLESS.MODULE_EXT.
#------------------------------------------------------------------------------
AC_DEFUN([CS_CHECK_STACKLESS],
    [AC_REQUIRE([CS_CHECK_PTHREAD])
    CS_COMPILER_IGNORE_LONG_DOUBLE([C++], [cs_cv_prog_cxx_ignore_long_double])

    AC_ARG_WITH([stackless],
	[AC_HELP_STRING([--]m4_if([$2],[without],[with],[without])[-stackless],
	    m4_if([$2],[without],[use],[do not use])
		m4_default([$3],[Stackless]))])
    AS_IF([test -z "$with_stackless"],
	[with_stackless=m4_if([$2], [without], [no], [yes])])

    CS_CHECK_PROGS([STACKLESS], [python])
    AC_SUBST([STACKLESS])
    CS_EMIT_BUILD_PROPERTY([STACKLESS],[$STACKLESS],[],[],CS_EMITTER_OPTIONAL([$1]))

    AS_IF([test -n "$STACKLESS" && test "$with_stackless" != no],
	[AC_CACHE_CHECK([for python SDK], [cs_cv_stackless_sdk],
	    [cs_pyver=`AC_RUN_LOG([$STACKLESS -c 'import sys, string; \
		print string.join(map(str,sys.version_info[[:2]]),".")'])`
	    cs_cv_pybase="python${cs_pyver}"

	    cs_cv_pybase_cflags=CS_RUN_PATH_NORMALIZE([$STACKLESS -c \
		'import distutils.sysconfig; \
		print "-I" + distutils.sysconfig.get_python_inc()'])
	    cs_cv_pybase_cflags="$cs_cv_pybase_cflags \
		$cs_cv_prog_cxx_ignore_long_double"

	    # Depending upon platform and installation, link library might
	    # reside in "get_python_lib()", "get_python_lib()/config",
	    # "${prefix}/lib" (Unix), or "${prefix}/libs" (Windows).
	    cs_cv_pybase_syslib=CS_RUN_PATH_NORMALIZE([$STACKLESS -c \
		'import distutils.sysconfig; \
		print distutils.sysconfig.get_python_lib(0,1)'])
	    cs_cv_pybase_sysprefix=CS_RUN_PATH_NORMALIZE([$STACKLESS -c \
		'import sys; print sys.prefix'])
	    cs_cv_pybase_lflags_base=''
	    _CS_CHECK_STACKLESS_LIBDIR([cs_cv_pybase_lflags_base],
		[cs_cv_pybase_syslib])
	    cs_cv_pybase_lflags_ext='$cs_cv_pybase_lflags_base'
	    _CS_CHECK_STACKLESS_LIBDIR([cs_cv_pybase_lflags_ext],
		[cs_cv_pybase_syslib], [config])
	    _CS_CHECK_STACKLESS_LIBDIR([cs_cv_pybase_sysprefix_lflags],
		[cs_cv_pybase_sysprefix], [lib])
	    _CS_CHECK_STACKLESS_LIBDIR([cs_cv_pybase_sysprefix_lflags],
		[cs_cv_pybase_sysprefix], [libs])

	    cs_cv_pybase_libs=CS_RUN_PATH_NORMALIZE([$STACKLESS -c \
		'import distutils.sysconfig; \
		print (distutils.sysconfig.get_config_var("LIBS") or "")+" "+ \
		      (distutils.sysconfig.get_config_var("SYSLIBS") or "")'])

	    cs_cv_stackless_ext=`AC_RUN_LOG([$STACKLESS -c \
		'import distutils.sysconfig; \
		print (distutils.sysconfig.get_config_var("SO") or "")'])`

	    AS_IF([test -n "$cs_pyver" &&
		   test -n "$cs_cv_pybase_cflags" &&
		   test -n "$cs_cv_pybase_lflags$cs_cv_pybase_sysprefix_lflags"],
		[cs_cv_stackless_sdk=yes], [cs_cv_stackless_sdk=no])])

	# Check if Python SDK is usable.  The most common library name is the
	# basename with a few decorations (for example, libpython2.2.a),
	# however some Windows libraries lack the decimal point (for example,
	# libpython22.a or python22.lib), so we must check for both variations.
	# Furthermore, MacOS/X 10.3 supplies a Python.framework, however,
	# earlier releases did not.  Instead, Python on MacOS/X pre-10.3 uses a
	# one-level linker namespace, which means that loadable Python modules
	# do not link against the Python library; instead, unresolved symbols
	# in the modules are satisfied automatically by the Python executable
	# when the module is loaded into the executable.  For this reason,
	# Python on MacOS/X does not even provide a Python link library.  We
	# account for this by trying -bundle, rather than linking against the
	# library.
	AS_IF([test $cs_cv_stackless_sdk = yes],
	    [CS_EMIT_BUILD_PROPERTY([STACKLESS.MODULE_EXT], [$cs_cv_stackless_ext],
		[], [], CS_EMITTER_OPTIONAL([$1]))
	    cs_pywinlib=`echo "$cs_cv_pybase" | sed 's/\.//g'`
	    cs_pyflags="$cs_pyflags CS_CREATE_TUPLE([],[],
	       [$cs_cv_pybase_lflags_base -framework Python])"
	    cs_pyflags="$cs_pyflags CS_CREATE_TUPLE([],[],
	       [$cs_cv_pybase_sysprefix_lflags -framework Python])"
	    cs_pyflags="$cs_pyflags CS_CREATE_TUPLE([],[],
	       [$cs_cv_pybase_lflags_ext -framework Python])"
	    
	    cs_pyflags="$cs_pyflags CS_CREATE_TUPLE([],[],
	       [$cs_cv_pybase_lflags_base -l$cs_cv_pybase])"
	    cs_pyflags="$cs_pyflags CS_CREATE_TUPLE([],[],
	       [$cs_cv_pybase_sysprefix_lflags -l$cs_cv_pybase])"
	    cs_pyflags="$cs_pyflags CS_CREATE_TUPLE([],[],
	       [$cs_cv_pybase_lflags_ext -l$cs_cv_pybase])"
	    
	    cs_pyflags="$cs_pyflags CS_CREATE_TUPLE([],[],
	       [$cs_cv_pybase_lflags_base -l$cs_pywinlib])"
	    cs_pyflags="$cs_pyflags CS_CREATE_TUPLE([],[],
	       [$cs_cv_pybase_sysprefix_lflags -l$cs_pywinlib])"
	    cs_pyflags="$cs_pyflags CS_CREATE_TUPLE([],[],
	       [$cs_cv_pybase_lflags_ext -l$cs_pywinlib])"
	    
	    cs_pyflags="$cs_pyflags CS_CREATE_TUPLE(
	       [],[-bundle -flat_namespace -undefined suppress],
	       [$cs_cv_pybase_lflags_base])"
	    cs_pyflags="$cs_pyflags CS_CREATE_TUPLE(
	       [],[-bundle -flat_namespace -undefined suppress],
	       [$cs_cv_pybase_sysprefix_lflags])"
	    cs_pyflags="$cs_pyflags CS_CREATE_TUPLE(
	       [],[-bundle -flat_namespace -undefined suppress],
	       [$cs_cv_pybase_lflags_ext])"
	       
	    CS_CHECK_BUILD([if python SDK is usable], [cs_cv_stackless],
		[AC_LANG_PROGRAM([[#include <Python.h>]],
		    [Py_Initialize(); Py_Finalize();])],
		[$cs_pyflags], [],
		[CS_EMIT_BUILD_RESULT([cs_cv_stackless], [STACKLESS],
		    CS_EMITTER_OPTIONAL([$1]))], [], [],
		[$cs_cv_pybase_cflags $cs_cv_sys_pthread_cflags],
		[                     $cs_cv_sys_pthread_lflags],
		[$cs_cv_pybase_libs   $cs_cv_sys_pthread_libs])

	    AS_IF([test $cs_cv_stackless = yes],
		CS_CHECK_BUILD([if python SDK is Stackless], [cs_cv_stackless_stackless],
			[AC_LANG_PROGRAM([[#include <stackless_api.h>]],
			[PyChannel_New(0);])],
			[$cs_pyflags], [],
			[CS_EMIT_BUILD_RESULT([cs_cv_stackless_stackless], [STACKLESS],
			CS_EMITTER_OPTIONAL([$1]))], [], [],
			[$cs_cv_pybase_cflags $cs_cv_sys_pthread_cflags],
			[                     $cs_cv_sys_pthread_lflags],
			[$cs_cv_pybase_libs   $cs_cv_sys_pthread_libs])],
		[cs_cv_stackless_stackless=no])
		
		cs_cv_stackless=$cs_cv_stackless_stackless],
	    [cs_cv_stackless=no]),

	[cs_cv_stackless=no])])

# _CS_CHECK_STACKLESS_LIBDIR(LFLAGS-VAR, DIR-VAR, [SUBDIR])
AC_DEFUN([_CS_CHECK_STACKLESS_LIBDIR],
    [AS_IF([test -d "$$2[]m4_ifval([$3],[/$3],[])"],
	[$1="$$1 -L$$2[]m4_ifval([$3],[/$3],[])"])])


#------------------------------------------------------------------------------
# CS_EMIT_CHECK_STACKLESS([SDK-CHECK-DEFAULT], [WITH-DESCRIPTION], [EMITTER])
#	DEPRECATED: Previously, layered EMITTER functionality atop
#	CS_CHECK_STACKLESS() before CS_CHECK_STACKLESS() supported emitters directly.
#------------------------------------------------------------------------------
AC_DEFUN([CS_EMIT_CHECK_STACKLESS],
    [CS_CHECK_STACKLESS(m4_ifval([$3], [$3], [emit]), [$1], [$2])])
