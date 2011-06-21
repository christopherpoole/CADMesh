AC_INIT([cadmesh], [1.0], [http://code.google.com/p/cadmesh/])

# Options to disable VCGLIB or TETGEN 
#AC_ARG_ENABLE(vcglib, [  --disable-vcglib        disable compilation against VCGLIB])
AC_ARG_ENABLE(tetgen, [  --disable-tetgen        disable compilation against TETGEN])

# Specify packages to include
AC_ARG_WITH(vcglib, [  --with-vcglib           specify VCGLIB source directory, default is ./vcglib])
AC_ARG_WITH(tetgen, [  --with-tetgen           specify TETGEN source directory, default is ./tetgen])

# Checks for C++ compiler
AC_PROG_CXX

if test "$with_vcglib" != ""
then
	VCGLIB_DIR=$with_vcglib
else
    AC_MSG_ERROR([VCGLIB location must be specified using --with-vcglib=dir])
fi

TETGEN_DIR=''
if test "$with_tetgen" != ""
then
	TETGEN_DIR=$with_tetgen
elif test "$with_tetgen" == ""
then
    if test "$enable_tetgen" != no
    then
        AC_LANG_PUSH([C++])
        AC_CHECK_HEADER(tetgen.h)
        AC_LANG_POP
        if test "$ac_cv_header_tetgen_h" == no
        then
	        AC_MSG_ERROR([tetgen.h missing, specify location using --with-tetgen=dir])
	    fi
    else
        TETGEN_DIR=''
    fi
fi

# Substitute source locations
AC_SUBST(vcglib_dir, $VCGLIB_DIR)

if test "$enable_tetgen" == no
then
    AC_SUBST(tetgen_lib, -ltet)
    AC_SUBST(disable_tetgen, -DNOTET=1)
else
    AC_SUBST(tetgen_lib, -ltet)
    AC_SUBST(disable_tetgen, '')
fi

AC_OUTPUT(Makefile)

