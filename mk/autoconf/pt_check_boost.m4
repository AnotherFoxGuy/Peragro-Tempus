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
#     http://www.nongnu.org/autoconf-archive/macros-by-category.html#BOOST
#
# LICENSE
#
#   Copyright (C) 2009 Development Team of Peragro Tempus
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

AC_DEFUN([PT_CHECK_BOOST],
[
    if test "x$want_boost" = "xyes"; then
        AX_BOOST_$1()
        CS_EMIT_BUILD_PROPERTY([BOOST.[$1].AVAILABLE], [$HAVE_BOOST_[$1]])
        CS_EMIT_BUILD_PROPERTY([BOOST.[$1].LFLAGS], [$BOOST_[$1]_LIB])
    fi
])
