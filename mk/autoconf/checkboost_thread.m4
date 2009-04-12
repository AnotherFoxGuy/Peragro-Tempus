AC_DEFUN([CS_CHECK_BOOST_THREAD],
[
    AC_REQUIRE([AX_BOOST_BASE])
if test "x$want_boost" = "xyes"; then

    CS_CHECK_LIB_WITH([boost_thread], [AC_LANG_PROGRAM([[#include <boost/thread.hpp>]],
        [boost::thread_group thrds; return 0;])], [], [C++],
        [CS_EMIT_BUILD_RESULT([cs_cv_libboost_thread], [BOOST.THREAD],
            CS_EMITTER_OPTIONAL([$1]))], [AC_MSG_ERROR([
*** boost_thread could not be found.
*** You can get it from http://www.boost.org
*** Take a look at http://wiki.peragro.org/index.php/Installation
*** Also check the LD_LIBRARY_PATH - should normally contain /usr/lib or /usr/lib64
*** You can also use --with-boost_thread=/path/to/boost
                ])],
            [$BOOST_CPPFLAGS],
            [],
            [$BOOST_LDFLAGS],
            [boost_thread-gcc, boost_thread-mingw])
fi
])
