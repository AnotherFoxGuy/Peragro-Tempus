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


  CS_CHECK_BUILD([if pthread_timedjoin_np available],
                 [pt_cv_pthread_timedjoin_np],
                 [AC_LANG_PROGRAM([#include <pthread.h>],
                                  [pthread_timedjoin_np(NULL, NULL, NULL);])],
                 [CS_CREATE_TUPLE([-DHAVE_PTHREAD_TIMEDJOIN_NP],[], [-lpthread])],
                                  [C++],
                                  [], [], [],
                                  [], [], [])

  AS_IF([test $pt_cv_pthread_timedjoin_np = yes],
	      [CS_EMIT_BUILD_PROPERTY([COMPILER.C++FLAGS.PTHREADNP],
                                [-DHAVE_PTHREAD_TIMEDJOIN_NP],
                                [+])])

])
