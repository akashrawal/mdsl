#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([MDSL],[0.0.0],[],[mdsl],[https://github.com/akashrawal/mdsl])
AC_CONFIG_SRCDIR([mdsl/mdsl.h]) #FIXME: filename
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([auxdir])
AM_INIT_AUTOMAKE([-Wall -Werror silent-rules])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])
AM_PROG_AR
LT_INIT

# Checks for programs.
#TODO: Enforce C11
AC_PROG_CC
AM_PROG_CC_C_O

#Write all output

AC_CONFIG_FILES([Makefile
                 data/Makefile
				 tests/Makefile
				 tests/logcc.sh
                 data/mdsl.pc
                 mdsl/Makefile])
AC_OUTPUT
