pix_DATA =     qtparted.xpm
pixdir =       $(prefix)/share/pixmaps

splash_DATA =  qtp_splash.png qtparted_16.png qtparted_32.png qtparted_64.png
splashdir =    $(prefix)/share/qtparted/pics

menu_DATA =    qtparted.desktop
menudir =      $(prefix)/share/applnk/System

script_DATA =  run_qtparted
scriptdir =    $(prefix)/sbin

install-data-hook:
	@for prog in $(script_DATA); do \
	    chmod +x $(DESTDIR)$(scriptdir)/$$prog; \
	done

EXTRA_DIST = $(pix_DATA) $(splash_DATA) $(menu_DATA) $(script_DATA) 

DISTCLEANFILES = run_qtparted
MAINTAINERCLEANFILES = Makefile.in run_qtparted
