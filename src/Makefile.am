SUBDIRS = 

sbin_PROGRAMS = qtparted

INCLUDES = -I. -I$(qt_includes) -I$(all_includes) 
KDE_OPTIONS = qtonly
INTERFACES = 
TRANSLATIONS = 

qtparted_METASOURCES = AUTO

qtparted_SOURCES = qp_ui_config.ui      \
				   qp_ui_create.ui      \
				   qp_ui_format.ui      \
				   qp_ui_progress.ui    \
				   qp_ui_resize.ui      \
				   qp_ui_devprop.ui     \
                   main.cpp             \
				   qp_dlgabout.cpp      \
				   qp_dlgconfig.cpp     \
				   qp_dlgcreate.cpp     \
				   qp_dlgformat.cpp     \
                   qp_dlgresize.cpp     \
				   qp_dlgprogress.cpp   \
				   qp_dlgdevprop.cpp    \
				   qp_drivelist.cpp     \
                   qp_devlist.cpp       \
				   qp_libparted.cpp     \
                   qp_partlist.cpp      \
				   qp_splash.cpp        \
				   qp_actlist.cpp       \
                   qp_extended.cpp      \
				   qp_listchart.cpp     \
				   qp_partition.cpp     \
				   qp_partwidget.cpp    \
				   qp_diskview.cpp      \
				   qp_window.cpp        \
                   qp_combospin.cpp     \
				   qp_exttools.cpp      \
				   qp_listview.cpp      \
                   qp_settings.cpp      \
				   qp_common.cpp        \
                   qp_filesystem.cpp    \
				   qp_navview.cpp       \
				   qp_fswrap.cpp        \
				   qp_sizepart.cpp      \
				   qp_spinbox.cpp       \
				   runprog.cpp          \
	                           debug.cpp            \
                   statistics.cpp

noinst_HEADERS =   qtparted.h           \
				   qp_dlgabout.h        \
				   qp_dlgconfig.h       \
				   qp_dlgcreate.h       \
				   qp_dlgformat.h       \
				   qp_dlgresize.h       \
                   qp_dlgprogress.h     \
                   qp_dlgdevprop.h      \
				   qp_drivelist.h       \
                   qp_devlist.h         \
				   qp_libparted.h       \
				   qp_partlist.h        \
				   qp_splash.h          \
                   qp_actlist.h         \
				   qp_extended.h        \
				   qp_listchart.h       \
				   qp_partition.h       \
                   qp_partwidget.h      \
                   qp_diskview.h        \
				   qp_window.h          \
				   qp_combospin.h       \
                   qp_exttools.h        \
				   qp_listview.h        \
				   qp_settings.h        \
				   qp_common.h          \
				   qp_filesystem.h      \
				   qp_navview.h         \
				   qp_fswrap.h          \
				   qp_sizepart.h        \
				   qp_spinbox.h         \
				   runprog.h            \
                   statistics.h

qtparted_LDFLAGS = @QTPARTED_LDFLAGS@ @QTPARTED_LIBS@ -L$(qt_libraries) $(all_libraries) $(LIBS)

qtparted_LDADD   = -ldl -lparted $(LIB_QPE) $(LIB_QT)

DISTCLEANFILES = $(piguicqt_METASOURCES)
MAINTAINERCLEANFILES = Makefile.in

%.uic.h: %.ui
	$(QT_UIC) -o $@ $<

%.uic.cpp: %.ui
	$(QT_UIC) -o $@ -impl $<c.h $<

%.moc.cpp: %.h
	$(QT_MOC) -o $@ $<

AM_CXXFLAGS=
CXXLINK = $(LIBTOOL) --mode=link $(CXX) $(AM_CXXFLAGS) $(CXXFLAGS) $(KDE_CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@ 
