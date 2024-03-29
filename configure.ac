AC_INIT([rpi-lcd], [0.4])
AC_SUBST(PACKAGE)
AC_SUBST(VERSION)

AM_INIT_AUTOMAKE([subdir-objects])

AC_CONFIG_SRCDIR([lib/rpi-lcd.c])
AC_CONFIG_MACRO_DIRS([m4])

AC_PROG_CC
AC_GNU_SOURCE
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
LT_INIT([disable-static])

PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

if test "${exec_prefix}" = "NONE"
then
  SYSTEMDDIR=${prefix}/lib/systemd
  DRACUTDIR=${prefix}/lib/dracut
else
  SYSTEMDDIR=${exec_prefix}/lib/systemd
  DRACUTDIR=${exec_prefix}/lib/dracut
fi
AC_SUBST(SYSTEMDDIR)
AC_SUBST(DRACUTDIR)

dnl enable additional compiler checks
AC_ARG_ENABLE([compiler-checks],
    AS_HELP_STRING([--enable-compiler-checks],[enables additional compiler checks, not for production]))

if test x"$enable_compiler_checks" = x"yes" ; then
   CFLAGS_CHECKS="-Wall -O2 -fsanitize=address -fno-omit-frame-pointer -g"
   LDFLAGS_CHECKS="-fsanitize=address"
fi

dnl enable additional compiler warnings
AC_ARG_ENABLE([compiler-warnings],
    AS_HELP_STRING([--enable-compiler-warnings],[enables additional compiler warnings]))

if test x"$enable_compiler_warnings" = x"yes" ; then
   if test x"$enable_compiler_checks" = x"yes" ; then
      AC_MSG_ERROR([Conflicting options: --enable-compiler-warnings cannot be used together with --eanble-compiler-checks])
   fi
   CFLAGS_WARNINGS="-O2 -g -Werror -W -Wall -DXTSTRINGDEFINES -D_FORTIFY_SOURCE=2 -fstack-protector-strong -funwind-tables -fasynchronous-unwind-tables -fstack-clash-protection -Werror=return-type -flto=8 -Wbad-function-cast -Wcast-align -Wcast-qual -Winline -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wshadow -Wstrict-prototypes -Wundef"
   LDFLAGS_WARNINGS=""
fi

AC_SUBST(CFLAGS_CHECKS)
AC_SUBST(LDFLAGS_CHECKS)
AC_SUBST(CFLAGS_WARNINGS)
AC_SUBST(LDFLAGS_WARNINGS)
AC_CONFIG_FILES([Makefile lib/Makefile src/Makefile etc/Makefile \
		 systemd/Makefile dracut/Makefile lib/librpi-lcd.pc])
AC_OUTPUT
