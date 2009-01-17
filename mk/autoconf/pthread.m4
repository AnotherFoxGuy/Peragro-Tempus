#==============================================================================
#    Copyright (C) 2008 Development Team of Peragro Tempus
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#==============================================================================

AC_DEFUN([PTHREAD_TIMEDJOIN_NP],[

  found=no

	AC_LANG_PUSH(C++)
     	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
	@%:@include <pthread.h>
	]], [[
  pthread_timedjoin_np(NULL, NULL, NULL);
	]])],[
        AC_MSG_RESULT(yes)
        found=yes
       	],[
       	])
	AC_LANG_POP([C++])

  if test $found != "no" ; then

    CS_EMIT_BUILD_FLAGS([if $found != "no"], [pt_cv_pthread_timedjoin_np],
      [CS_CREATE_TUPLE([-DHAVE_PTHREAD_TIMEDJOIN_NP])], [C], [COMPILER.LFLAGS], [+])
  fi
])
