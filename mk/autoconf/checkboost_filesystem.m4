AC_DEFUN([CS_CHECK_BOOST_FILESYSTEM],
[
	AC_REQUIRE([AX_BOOST_BASE])
if test "x$want_boost" = "xyes"; then

	CS_CHECK_LIB_WITH([boost_filesystem], [AC_LANG_PROGRAM([[#include <boost/filesystem/path.hpp>]],
		    [boost::filesystem::path test("configure");])], [], [C++],
		    [CS_EMIT_BUILD_RESULT([cs_cv_libboost_filesystem], [BOOST.FILESYSTEM],
	                    CS_EMITTER_OPTIONAL([$1]))], [AC_MSG_ERROR([
*** boost_filesystem could not be found.
*** You can get it from http://www.boost.org
*** Take a look at http://wiki.peragro.org/index.php/Installation
*** Also check the LD_LIBRARY_PATH - should normally contain /usr/lib or /usr/lib64
*** You can also use --with-boost_filesystem=/path/to/boost
                      ])],
		    [$BOOST_CPPFLAGS],
		    [],
		    [$BOOST_LDFLAGS],
		    [boost_filesystem-gcc, boost_filesystem-mingw])
fi
])
