SUBDIRS = src data ts doc
EXTRA_DIST = AUTHORS COPYING ChangeLog INSTALL README TODO qtparted.lsm 
AUTOMAKE_OPTIONS = foreign 

autoconf:
	@libtoolize --copy --force
	@autoheader
	@aclocal
	@automake -ac
	@autoconf
