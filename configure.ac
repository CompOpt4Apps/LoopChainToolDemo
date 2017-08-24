#                                               -*- Autoconf -*-
#
#       _____     __             __
#      / ___/__  / /  ___  _____/ /_
#     / /__/ _ \/ _ \/ _ \/ _/_  __/
#     \___/\___/_//_/\___/_/  /_/
#     Please refer to Copyright.txt, in Chohort's root directory.
#
#

#--------------------------------------------------------------------
# Process this file with autoconf to produce a configure script.
#--------------------------------------------------------------------

AC_PREREQ([2.59])
AC_INIT([Cohort], [0.9], [Stephen.Guzik@colostate.edu], [Cohort.tar.bz2])
AC_CONFIG_SRCDIR([lib/src/BaseTools/CHTmsg.cpp])

#--------------------------------------------------------------------
# Created files
#--------------------------------------------------------------------

AC_CONFIG_HEADERS([lib/src/BaseTools/CHTconfig.H])
AC_CONFIG_FILES([mk/Make.defs])

#--------------------------------------------------------------------
# System information
#--------------------------------------------------------------------

AC_CANONICAL_HOST
have_x86=`expr $host : '.*\(86\).*'`
AC_CHECK_SIZEOF([void *])
if test "$ac_cv_sizeof_void_p" -eq "8"; then
  ch_int_systemptrsize="64"
elif test "$ac_cv_sizeof_void_p" -eq "4"; then
  ch_int_systemptrsize="32"
fi

#--------------------------------------------------------------------
# Top directory
#--------------------------------------------------------------------

AC_SUBST([TOPDIR],[`pwd`])

#--------------------------------------------------------------------
# Get the C++ compiler
#--------------------------------------------------------------------

AC_PROG_CXX([$CXX])

#--------------------------------------------------------------------
# Standard checks for required programs
#--------------------------------------------------------------------

AC_PROG_INSTALL

#--------------------------------------------------------------------
# Need flex and bison for the parser.
#--------------------------------------------------------------------

# Stream editor
#AC_ARG_VAR([SED], [Stream editor])
#AC_CHECK_PROG([SED], [sed], [sed], [])
AC_PROG_SED

# C shell
AC_ARG_VAR([CSHELLCMD], [C shell])
AC_CHECK_PROG([CSHELLCMD], [csh], [csh -f -c])

# Library archiver
AC_ARG_VAR([AR], [Library archiver])
AC_CHECK_PROG([AR], [ar], [ar rcs])

# Perl
AC_ARG_VAR([PERL], [Perl])
AC_CHECK_PROG([PERL], [perl], [perl])

# Flex
AC_PROG_LEX

# Bison
AC_ARG_VAR([BISON], [GNU parser])
AC_CHECK_PROG([BISON], [bison], [bison])
AC_MSG_CHECKING([for bison major version number])
BISON_MAJOR=`$BISON --version 2>&1 | $SED '1{s/.* \(@<:@0-9@:>@\)\..*/\1/;q}'`
AC_MSG_RESULT($BISON_MAJOR)
AC_SUBST([_bison_major], [$BISON_MAJOR])
AC_DEFINE_UNQUOTED([CHTDEF_BISON_MAJOR], [$BISON_MAJOR])

#--------------------------------------------------------------------
# Create files
#--------------------------------------------------------------------

AC_OUTPUT
