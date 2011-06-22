####################################################
# GEANT4 CAD INTERFACE
#
# File:      configur.ac - autoconf
#
# Author:    Christopher M Poole,
# Email:     mail@christopherpoole.net
#
# Date:      22nd April, 2011
####################################################

AC_INIT([cadmesh], [1.0], [http://code.google.com/p/cadmesh/])

# Options to disable VCGLIB or TETGEN 
AC_ARG_ENABLE(vcglib, [  --disable-vcglib        disable compilation against VCGLIB])
AC_ARG_ENABLE(tetgen, [  --disable-tetgen        disable compilation against TETGEN])

if test "$enable_vcglib" == no
then
    if test "$enable_tetgen" == no
    then
        AC_MSG_ERROR([Both VCGLIB and TETGEN cannot be disabled!])
    fi
fi

# Specify packages to include
AC_ARG_WITH(vcglib, [  --with-vcglib=dir         specify VCGLIB source directory])
AC_ARG_WITH(tetgen, [  --with-tetgen=dir         specify TETGEN source directory])

# VCGLIB location
if test "$enable_vcglib" != no
then
    if test "$with_vcglib" == yes || test "$with_vcglib" == ''
    then
        AC_MSG_ERROR([Specify VCGLIB location using --with-vcglib=dir OR disable using --disable-vcglib])
    else
	    VCGLIB_DIR=$with_vcglib
    fi
fi

# TETGEN location
if test "$enable_tetgen" != no
then
    AC_LANG_PUSH([C++])
    AC_CHECK_HEADER(tetgen.h)
    AC_LANG_POP
    if test "$ac_cv_header_tetgen_h" == no
    then
        if test "$with_tetgen" == yes || test "$with_tetgen" == ''
        then
            AC_MSG_ERROR([Specify TETGEN location using --with-tetgen=dir OR disable using --disable-tetgen])
        else
	        TETGEN_DIR=$with_tetgen
        fi
    fi
fi

# Checks for C++ compiler
AC_PROG_CXX

# Substitute source locations
if test "$enable_vcglib" == no
then
    AC_SUBST(include_flag, '')
    AC_SUBST(vcglib_dir, '')
    AC_SUBST(plylib_object, '')
    AC_SUBST(disable_vcglib, -DNOVCGLIB=1)
else
    AC_SUBST(include_flag, '-I')
    AC_SUBST(vcglib_dir, $VCGLIB_DIR)
    AC_SUBST(plylib_object, /wrap/ply/plylib.o)
fi


if test "$enable_tetgen" == no
then
    AC_SUBST(tetgen_lib, '')
    AC_SUBST(disable_tetgen, -DNOTET=1)
else
    AC_SUBST(tetgen_lib, -ltet)
    AC_SUBST(disable_tetgen, '')
fi

AC_OUTPUT(Makefile)

