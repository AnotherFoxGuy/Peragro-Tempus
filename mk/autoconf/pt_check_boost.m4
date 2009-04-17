# DESCRIPTION
#
#   Wrapper around macros to check for a boost library, and emit to the CS
#   build system. For example:
#
#     PT_CHECK_BOOST(THREAD)
#
#   which will call:
#
#     AX_BOOST_THREAD()
#
#   which can be found in the file ax_boost_thread.m4, from:
#
#     http://autoconf-archive.cryp.to/macros-by-category.html#BOOST
#

AC_DEFUN([PT_CHECK_BOOST],
[
    if test "x$want_boost" = "xyes"; then
        AX_BOOST_$1()
        CS_EMIT_BUILD_PROPERTY([BOOST.[$1].AVAILABLE], [$HAVE_BOOST_[$1]])
        CS_EMIT_BUILD_PROPERTY([BOOST.[$1].LFLAGS], [$BOOST_[$1]_LIB])
    fi
])
