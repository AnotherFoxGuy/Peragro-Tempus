AC_DEFUN([CS_CHECK_BOOST_PYTHON],
[
	AC_REQUIRE([AX_BOOST_BASE])
	AC_REQUIRE([CS_CHECK_STACKLESS])

	CS_CHECK_LIB_WITH([boost_python], [AC_LANG_PROGRAM([[#include <boost/python.hpp>
                     using namespace boost::python;
		     const char* testMe() { return "Test"; }
                     BOOST_PYTHON_MODULE(test) {
		       def("testMe", testMe);
		     }]], [])], [], [C++],
		    [CS_EMIT_BUILD_RESULT([cs_cv_libboost_python], [BOOST.PYTHON],
	                    CS_EMITTER_OPTIONAL([$1]))], [],
		    [$BOOST_CPPFLAGS        $cs_cv_stackless_cflags],
		    [$cs_cv_stackless_lflags],
		    [$BOOST_LDFLAGS         $cs_cv_stackless_libs],
		    [boost_python-gcc, boost_python-mingw])
])
