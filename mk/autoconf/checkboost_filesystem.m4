AC_DEFUN([CS_CHECK_BOOST_FILESYSTEM],
[
	AC_REQUIRE([AX_BOOST_BASE])

	CS_CHECK_LIB_WITH([boost_filesystem], [AC_LANG_PROGRAM([[#include <boost/filesystem/path.hpp>]],
		    [boost::filesystem::path test("configure");])], [], [C++],
		    [CS_EMIT_BUILD_RESULT([cs_cv_libboost_filesystem], [BOOST.FILESYSTEM],
	                    CS_EMITTER_OPTIONAL([$1]))], [],
		    [$BOOST_CPPFLAGS],
		    [],
		    [$BOOST_LDFLAGS],
		    [boost_filesystem-gcc, boost_filesystem-mingw])
])
