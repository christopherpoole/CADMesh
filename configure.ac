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

# Options to disable VCGLIB or TETGEN or enable DEBUG
AC_ARG_ENABLE(vcglib, [  --disable-vcglib        disable compilation against VCGLIB])
AC_ARG_ENABLE(tetgen, [  --disable-tetgen        disable compilation against TETGEN])
AC_ARG_ENABLE(collada,[  --enable-collada        enable support for COLLADA files, requires compilation against QT/QtXml])
AC_ARG_ENABLE(debug,  [  --enable-debug          enable debugging in cadmesh])

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
AC_ARG_WITH(qt,[  --with-qt=dir            specify QT source directory])

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

# COLLADA support (QT location)
if test "$enable_collada" != no
then
    if test "$with_qt" == yes || test "$with_qt" == ''
    then
        AC_MSG_ERROR([Specify QT location using --with-qt=dir OR disable COLLADA support using --disable-collada])
    else
	    QT_DIR=$with_qt
    fi
fi

# Checks for C++ compiler
AC_PROG_CXX

# Substitute source locations
if test "$enable_vcglib" == no
then
    AC_SUBST(vcglib_incflag, '')
    AC_SUBST(vcglib_dir, '')
    AC_SUBST(plylib_object, '')
    AC_SUBST(disable_vcglib, -DNOVCGLIB)
else
    AC_SUBST(vcglib_incflag, '-I')
    AC_SUBST(vcglib_dir, $VCGLIB_DIR)
    AC_SUBST(plylib_object, /wrap/ply/plylib.o)
    AC_SUBST(disable_vcglib, '')
fi

if test "$enable_tetgen" == no
then
    AC_SUBST(tetgen_incflag, '')
    AC_SUBST(tetgen_dir, '')
    AC_SUBST(tetgen_lib, '')
    AC_SUBST(disable_tetgen, -DNOTET)
else
    if test "$ac_cv_header_tetgen_h" == no
    then
        AC_SUBST(tetgen_incflag, '-I')
        AC_SUBST(tetgen_dir, $TETGEN_DIR)
    else
        AC_SUBST(tetgen_incflag, '')
        AC_SUBST(tetgen_dir, '')
    fi
    AC_SUBST(tetgen_lib, -ltet)
    AC_SUBST(disable_tetgen, '')
fi

if test "$enable_collada" == no
then
    AC_SUBST(qt_incflag, '')
    AC_SUBST(qt_dir, '')
    AC_SUBST(qt_subdir, '')
    AC_SUBST(qtxml_lib, '')
    AC_SUBST(disable_collada, -DNOCOLLADA)
else
    AC_SUBST(qt_incflag, '-I')
    AC_SUBST(qt_dir, $QT_DIR)
    AC_SUBST(qt_subdir, '/Qt')
    AC_SUBST(qtxml_lib, -lQtXml)
    AC_SUBST(disable_collada, '')
fi

if test "$enable_debug" == yes
then
    AC_SUBST(debug, -DDEBUG)
else
    AC_SUBST(debug, '')
fi


AC_OUTPUT(Makefile)
AC_OUTPUT(cadmesh.gmk)

