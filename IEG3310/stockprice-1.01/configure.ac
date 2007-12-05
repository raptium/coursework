AC_PREREQ(2.61)
AC_INIT([stockprice],[1.01],[raptium@gmail.com])
AM_INIT_AUTOMAKE([-Wall])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])


AC_PROG_CC
AC_PROG_RANLIB

AC_CHECK_LIB([bz2],[BZ2_bzBuffToBuffCompress],,[AC_MSG_ERROR("libbz2 not found! You must have libbz2 installed to compile me.")])
AC_CHECK_LIB([ncurses],[initscr])

AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h unistd.h],,[AC_MSG_ERROR("You do not have the necessary headers to compile me!!")])

AC_C_CONST
AC_STRUCT_TM

AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([gethostbyname inet_ntoa memset socket])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
