dnl QtParted: Nice partition tool
dnl http://qtparted.sourceforge.net

AC_PREREQ(2.53)

AC_INIT([qtparted],[0.4.5-cvs],[zanac4ever@virgilio.it])
AC_CONFIG_SRCDIR([configure.ac])
AM_CONFIG_HEADER(config.h)
AC_PREFIX_DEFAULT([/usr])
AC_CANONICAL_SYSTEM
AC_DEFINE([HOMEPAGE],["http://qtparted.sourceforge.net/"],["official web site"])

PACKAGE=qtparted
VERSION=0.4.5-cvs
HOMEPAGE="http://qtparted.sourceforge.net/"
AM_INIT_AUTOMAKE($PACKAGE, $VERSION)
 
dnl for bytesex stuff
AC_C_BIGENDIAN

dnl Check for programs.
AC_ISC_POSIX
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_GCC_TRADITIONAL

dnl Check for typedefs, structures and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_STRUCT_TM

dnl AC_PROG_LIBTOOL
KDE_PROG_LIBTOOL

case "$host_os" in
    linux*)
        OS=linux ;;
    gnu*)
        OS=gnu ;;
    bsdi*)
        OS=bsdi;;
    darwin*)
        OS=darwin;;
    *mingw32*)
        OS=mingw32;;
    *)
        AC_MSG_ERROR(unknown or unsupported OS \"$host_os\".  Only linux and gnu are supported.);;
esac

AC_SUBST(OS)

QTPARTED_PREFIX="${prefix}"
AC_SUBST(QTPARTED_PREFIX)

dnl with-log-dir option.
AC_ARG_WITH(log-dir,
  [  --with-log-dir=PATH     logfiles [/var/log]],
    LOGDIR=$with_log_dir, LOGDIR=/var/log)
    AC_SUBST(LOGDIR)

dnl Check for types sizes.
dnl AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(void *)
AC_CHECK_SIZEOF(long long)

dnl with-debug-level option.
AC_ARG_WITH(debug-level,
  [  --with-debug-level=X    default debut level used to write the debug logfile [1]],
   DEFAULT_DEBUG_LEVEL=$with_debug_level, DEFAULT_DEBUG_LEVEL=1
)
AC_SUBST(DEFAULT_DEBUG_LEVEL)
AC_SUBST(HOMEPAGE)


dnl Libparted location
AC_ARG_WITH(parted-lib,[  --with-parted-lib=DIR   Parted library location],
    LIBS="$LIBS -L$withval"
)
AC_ARG_WITH(parted-headers,[  --with-parted-headers=DIR  Parted include files location],
  CPPFLAGS="$CPPFLAGS -I$withval"
)

dnl options for developers
AC_ARG_ENABLE(devel,[  --enable-devel          Enable developers options (debug, ...)],
    devel_support=$enableval,
    devel_support=no
)
if test "$devel_support" = "yes" ; then
  AC_DEFINE([DEVEL_SUPPORT], ["enabled"], ["Enable if you are a developer"])
fi

dnl option Qt-3.0 support
AC_ARG_ENABLE(qt30support,[  --enable-qt30support    If you have the old Qt-3.0 version, you have to enable it],
    qt30_support=$enableval,
    qt30_support=no
)
if test "$qt30_support" = "yes" ; then
  AC_DEFINE([QT30COMPATIBILITY],[],["Qt 3.0 support"])
fi

dnl AM_KDE_WITH_NLS

dnl Checking for libparted. (FIXME for libparted detection)
dnl PARTED_CHECK_LIBPARTED(1.6.0, , [AC_MSG_NOTICE([*** libparted >= 1.6.0 not installed - please install first ***])])
dnl PARTED_CHECK_LIBPARTED(1.6.0, , [AC_MSG_ERROR([*** libparted >= 1.6.0 not installed - please install first ***])])
dnl AC_SUBST(PARTED_LIBS)
AC_CHECK_LIB(uuid, uuid_generate, [], AC_MSG_ERROR([*** uuid library (libuuid) not found]))
AC_CHECK_LIB(dl, dlopen,          [], AC_MSG_ERROR([*** dl library (libdl) not found]))
AC_CHECK_LIB(parted, ped_malloc,  [], AC_MSG_ERROR([*** libparted >= 1.6.0 not installed - please install first ***]))

dnl options for volume-name-labels
AC_ARG_ENABLE(labels,[  --enable-labels       Enable volume name labels support (needs parted-1.6.7)],
    labels_support=$enableval,
    labels_support=no
)

dnl options for ntfs
AC_ARG_ENABLE(ntfs,[  --disable-ntfs          Disable the ntfs support (you can use this option if ntfsprogs is missing)],
    ntfs_support=$enableval,
    ntfs_support=yes
)

dnl options for ext3fs
AC_ARG_ENABLE(ext3fs,[  --disable-ext3fs        Disable the ext3fs support (you can use this option if e2fsprogs is missing)],
    ext3fs_support=$enableval,
    ext3fs_support=yes
)

dnl options for jfs
AC_ARG_ENABLE(jfs,[  --disable-jfs           Disable the jfs support (you can use this option if jfsutils is missing)],
    jfs_support=$enableval,
    jfs_support=yes
)

dnl options for xfs
AC_ARG_ENABLE(xfs,[  --disable-xfs           Disable the xfs support (you can use this option if xfsprogs is missing)],
    xfs_support=$enableval,
    xfs_support=yes
)

dnl options for libreiserfs
AC_ARG_ENABLE(reiserfs,[  --disable-reiserfs      Disable the reiserfs support (you can use this option if progsreiserfs is missing)],
    reiserfs_support=$enableval,
    reiserfs_support=yes
)
if test "$reiserfs_support" = "yes" ; then
  AC_CHECK_LIB(reiserfs, reiserfs_fs_create, [], AC_MSG_ERROR([*** reiserfsprogs library (libreiserfs) not found]))
  FILESYS_REISERFS_SUPPORT=1
  QTPARTED_LIBS="$QTPARTED_LIBS -lreiserfs"
else
  FILESYS_REISERFS_SUPPORT=0
fi

dnl paths for programs
AC_PATH_PROG(FSPATH_MKNTFS,         mkntfs, missing, $PATH:/usr/sbin:/usr/bin:/sbin:/usr/local/sbin)
AC_PATH_PROG(FSPATH_NTFSRESIZE,     ntfsresize, missing, $PATH:/usr/sbin:/usr/bin:/sbin:/usr/local/sbin)
AC_PATH_PROG(FSPATH_MKFSEXT3,       mkfs.ext3, missing, $PATH:/usr/sbin:/usr/bin:/sbin:/usr/local/sbin)
AC_PATH_PROG(FSPATH_MKFSJFS,        mkfs.jfs, missing, $PATH:/usr/sbin:/usr/bin:/sbin:/usr/local/sbin)
AC_PATH_PROG(FSPATH_MKFSXFS,        mkfs.xfs, missing, $PATH:/usr/sbin:/usr/bin:/sbin:/usr/local/sbin)
AC_PATH_PROG(FSPATH_MOUNT,          mount, missing, $PATH:/usr/sbin:/usr/bin:/sbin:/usr/local/sbin)
AC_PATH_PROG(FSPATH_UMOUNT,         umount, missing, $PATH:/usr/sbin:/usr/bin:/sbin:/usr/local/sbin)
AC_PATH_PROG(FSPATH_XFS_GROWFS,     xfs_growfs, missing, $PATH:/usr/sbin:/usr/bin:/sbin:/usr/local/sbin)

if test "$ntfs_support" = "yes" && test "$FSPATH_MKNTFS" = "missing" ; then
  AC_MSG_ERROR([*** mkntfs not found. Please, install ntfsprogs (http://linux-ntfs.sourceforge.net) or disable the NTFS support with  --disable-ntfs])
  FILESYS_NTFS_SUPPORT=0
else
  if test "$ntfs_support" = "yes"; then
    FILESYS_NTFS_SUPPORT=1
  else
    FILESYS_NTFS_SUPPORT=0
  fi
fi

if test "$ext3fs_support" = "yes" && test "$FSPATH_MKFSEXT3" = "missing"; then
  AC_MSG_ERROR([*** mkfs.ext3 not found. Please, install e2fsprogs (http://e2fsprogs.sourceforge.net/) or disable the ext3fs support with --disable-ext3fs])
  FILESYS_EXT3_SUPPORT=0
else
  if test "$ext3fs_support" = "yes" ; then
    FILESYS_EXT3FS_SUPPORT=1
  else
    FILESYS_EXT3FS_SUPPORT=0
  fi
fi

if test "$jfs_support" = "yes" && test "$FSPATH_MKFSJFS" = "missing"; then
  AC_MSG_ERROR([*** mkfs.jfs not found. Please, install jfsprogs (http://oss.software.ibm.com/jfs/) or disable the jfs support with --disable-jfs])
  FILESYS_JFS_SUPPORT=0
else
  if test "$jfs_support" = "yes" ; then
    FILESYS_JFS_SUPPORT=1
  else
    FILESYS_JFS_SUPPORT=0
  fi
fi

if test "$xfs_support" = "yes" && test "$FSPATH_MKFSXFS" = "missing"; then
  AC_MSG_ERROR([*** mkfs.xfs not found. Please, install xfsprogs (http://oss.sgi.com/projects/xfs/) or disable the xfs support with --disable-xfs])
  FILESYS_XFS_SUPPORT=0
else
  if test "$xfs_support" = "yes" ; then
    FILESYS_XFS_SUPPORT=1
  else
    FILESYS_XFS_SUPPORT=0
  fi
fi

AC_SUBST(FSPATH_MKNTFS)
AC_SUBST(FSPATH_NTFSRESIZE)
AC_SUBST(FSPATH_MKFSEXT3)
AC_SUBST(FSPATH_MKFSJFS)
AC_SUBST(FSPATH_MKFSXFS)
AC_SUBST(FSPATH_MOUNT)
AC_SUBST(FSPATH_UMOUNT)
AC_SUBST(FSPATH_XFS_GROWFS)

AC_SUBST(FILESYS_REISERFS_SUPPORT)
AC_SUBST(FILESYS_EXT3FS_SUPPORT)
AC_SUBST(FILESYS_NTFS_SUPPORT)
AC_SUBST(FILESYS_JFS_SUPPORT)
AC_SUBST(FILESYS_XFS_SUPPORT)

dnl KDE_USE_QT(3)
AC_ENABLE_SHARED(yes)
AC_PATH_QT(3.0)

PATH_QTDIR="$QTDIR"
AC_SUBST(PATH_QTDIR)

dnl Check for Qt "lrelease" translation program
AC_PATH_PROG(QTPATH_LRELEASE,       lrelease, missing)

if test "$QTPATH_LRELEASE" = "missing" ; then
  AC_MSG_ERROR([*** lrelease (Qt translation compiler) not found. Please, reinstall Qt])
fi

dnl export Qt paths
AC_SUBST(all_libraries)
AC_SUBST(all_includes)
AC_SUBST(qt_libraries)
AC_SUBST(qt_includes)

dnl set compilation variables
CPPFLAGS="$CPPFLAGS -fexceptions $QTE_NORTTI -Wall -Wno-deprecated" dnl "--no-rtti"
DEFS="$DEFS -D_REENTRANT -D_FILE_OFFSET_BITS=64"

dnl Static executables?
dnl AC_ARG_ENABLE(static,[  --enable-static         build static binaries],
dnl     exe_static=$enableval,
dnl     exe_static=no
dnl )

dnl if test "$exe_static" = "yes" ; then
dnl  QTPARTED_LDFLAGS="$QTPARTED_LDFLAGS"
AC_SUBST(QTPARTED_LDFLAGS)
dnl fi

AC_SUBST(QTPARTED_LIBS)

dnl This test must come as early as possible after the compiler configuration
dnl tests, because the choice of the file model can (in principle) affect
dnl whether functions and headers are available, whether they work, etc.
AC_SYS_LARGEFILE

dnl Initialize i18n:
dnl ALL_LINGUAS="br da de es fr gl it nl pl"
dnl AM_GNU_GETTEXT
dnl localedir=${datadir}/locale
dnl AC_SUBST(localedir)

top_srcdir=$PWD
AC_SUBST(top_srcdir)

dnl ******************************
dnl *** output the whole stuff ***
dnl ******************************

AC_OUTPUT_COMMANDS([

## Generate `qptypes.h' in two cases
## 1. `config.status' is run either explicitly, or via configure.
##     Esp. not when it is run in `Makefile' to generate makefiles and
##     config.h
## 2. CONFIG_OTHER is set explicitly
##
## Case 1 is difficult.  We know that `automake' sets one of
## CONFIG_FILES or CONFIG_HEADERS to empty.  This heuristic works
## only when AM_CONFIG_HEADER is set, however.

if test -n "${CONFIG_FILES}" && test -n "${CONFIG_HEADERS}"; then
  # Both CONFIG_FILES and CONFIG_HEADERS are non-empty ==> Case 1
  CONFIG_OTHER=${CONFIG_OTHER:-qptypes.h}
fi
case "$CONFIG_OTHER" in
*qptypes.h*)
	echo creating qptypes.h
	outfile=qptypes.h-tmp
	cat > $outfile <<\_______EOF
/* qptypes.h
 *
 * This is a generated file.  Please modify `configure.in'
 */

#ifndef _QPTYPES_H
#define _QPTYPES_H

typedef signed char gint8;
typedef signed char s8;
typedef unsigned char guint8;
typedef unsigned char u8;
typedef unsigned char BYTE;
typedef unsigned char BOOL;
_______EOF


	if test -n "$gint16"; then
	  cat >>$outfile <<_______EOF
typedef signed $gint16 gint16;
typedef signed $gint16 s16;
typedef unsigned $gint16 guint16;
typedef unsigned $gint16 u16;
typedef unsigned $gint16 WORD;
_______EOF
	fi


	if test -n "$gint32"; then
	  cat >>$outfile <<_______EOF
typedef signed $gint32 gint32;
typedef signed $gint32 s32;
typedef unsigned $gint32 guint32;
typedef unsigned $gint32 u32;
typedef unsigned $gint32 DWORD;
_______EOF
	fi


	if test -n "$gint64"; then
	  cat >>$outfile <<_______EOF
${glib_warning_guard}#define G_HAVE_GINT64 1

${glib_extension}typedef signed $gint64 gint64;
${glib_extension}typedef signed $gint64 s64;
${glib_extension}typedef unsigned $gint64 u64;
${glib_extension}typedef unsigned $gint64 guint64;
${glib_extension}typedef unsigned $gint64 QWORD;

#define $g_byte_order

#define G_GINT64_CONSTANT(val)	$gint64_constant
_______EOF
	fi


	if test -z "$glib_unknown_void_p"; then
	  cat >>$outfile <<_______EOF

_______EOF
	else
	  echo '#error SIZEOF_VOID_P unknown - This should never happen' >>$outfile
	fi



	cat >>$outfile <<_______EOF
#endif /* _QPTYPES_H_ */
_______EOF


	if cmp -s $outfile qptypes.h; then
	  echo qptypes.h is unchanged
	  rm -f $outfile
	else
	  mv $outfile qptypes.h
	fi ;;
esac
],[

case 2 in
$ac_cv_sizeof_short)		gint16=short;;
$ac_cv_sizeof_int)		gint16=int;;
esac
case 4 in
$ac_cv_sizeof_long)		gint32=long;;
$ac_cv_sizeof_short)		gint32=short;;
$ac_cv_sizeof_int)		gint32=int;;
esac
case 8 in
$ac_cv_sizeof_int)
  gint64=int
  glib_extension=
  glib_warning_guard=
  gint64_constant='(val)'
  ;;
$ac_cv_sizeof_long)
  gint64=long
  glib_extension=
  glib_warning_guard=
  gint64_constant='(val##L)'
  ;;
$ac_cv_sizeof_long_long)
  gint64='long long'
  glib_extension='G_GNUC_EXTENSION '
  glib_warning_guard="
#if defined (__GNUC__) && (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 8))
#  define G_GNUC_EXTENSION __extension__
#else
#  define G_GNUC_EXTENSION
#endif

"
  gint64_constant='(G_GNUC_EXTENSION (val##LL))'
  ;;
esac

case xyes in
x$ac_cv_c_bigendian)
  g_byte_order=ENDIAN_BIG
  ;;
*)
  g_byte_order=ENDIAN_LITTLE
  ;;
esac
])

AC_CONFIG_FILES([
	Makefile	
])
AC_CONFIG_FILES([
	src/Makefile
])
AC_CONFIG_FILES([
	src/qp_options.h
])
AC_CONFIG_FILES([
	data/Makefile
])
AC_CONFIG_FILES([
	data/run_qtparted
])
AC_CONFIG_FILES([
	ts/Makefile
])
AC_CONFIG_FILES([
	doc/Makefile
])
AC_CONFIG_FILES([
	redhat/qtparted-x11.spec
])
AC_CONFIG_FILES([
	redhat/qtparted-embedded.spec
])

AC_OUTPUT

echo "
global configuration
--------------------
system                  : \"${OS}\"
PREFIX                  : \"${QTPARTED_PREFIX}\"
QTPARTED_LDFLAGS        : \"${QTPARTED_LDFLAGS}\"
QTPARTED_LIBS           : \"${QTPARTED_LIBS}\"
QTDIR                   : \"${PATH_QTDIR}\"
Qt translation compiler : \"${QTPATH_LRELEASE}\"

supported file systems (0=no, 1=yes)
------------------------------------
ReiserFS                : ${FILESYS_REISERFS_SUPPORT}
Ext2fs/ext3fs           : ${FILESYS_EXT3FS_SUPPORT}
Windows NTFS:           : ${FILESYS_NTFS_SUPPORT}
Ibm JFS:                : ${FILESYS_JFS_SUPPORT}
Sgi XFS:                : ${FILESYS_XFS_SUPPORT}

Type \"make\" to compile QtParted
"
