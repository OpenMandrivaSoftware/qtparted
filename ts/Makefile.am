EXTRA_DIST = qtparted_cs.qm qtparted_de.qm qtparted_es.qm qtparted_fr.qm qtparted_it.qm qtparted_pl.qm qtparted_ru.qm qtparted_ua.qm qtparted_ca.qm

trans_DATA = qtparted_cs.qm qtparted_de.qm qtparted_es.qm qtparted_fr.qm qtparted_it.qm qtparted_pl.qm qtparted_ru.qm qtparted_ua.qm qtparted_ca.qm
transdir   = $(prefix)/share/qtparted/locale

qtparted_cs.qm:
	lupdate -verbose ../src/*.cpp ../src/*.h ../src/*.ui -ts qtparted_cs.ts
	lrelease qtparted_cs.ts

qtparted_de.qm:
	lupdate -verbose ../src/*.cpp ../src/*.h ../src/*.ui -ts qtparted_de.ts
	lrelease qtparted_de.ts

qtparted_es.qm:
	lupdate -verbose ../src/*.cpp ../src/*.h ../src/*.ui -ts qtparted_es.ts
	lrelease qtparted_es.ts

qtparted_fr.qm:
	lupdate -verbose ../src/*.cpp ../src/*.h ../src/*.ui -ts qtparted_fr.ts
	lrelease qtparted_fr.ts

qtparted_it.qm:
	lupdate -verbose ../src/*.cpp ../src/*.h ../src/*.ui -ts qtparted_it.ts
	lrelease qtparted_it.ts

qtparted_pl.qm:
	lupdate -verbose ../src/*.cpp ../src/*.h ../src/*.ui -ts qtparted_pl.ts
	lrelease qtparted_pl.ts

qtparted_ru.qm:
	lupdate -verbose ../src/*.cpp ../src/*.h ../src/*.ui -ts qtparted_ru.ts
	lrelease qtparted_ru.ts

qtparted_ua.qm:
	lupdate -verbose ../src/*.cpp ../src/*.h ../src/*.ui -ts qtparted_ua.ts
	lrelease qtparted_ua.ts

qtparted_ca.qm:
	lupdate -verbose ../src/*.cpp ../src/*.h ../src/*.ui -ts qtparted_ca.ts
	lrelease qtparted_ca.ts

CLEANFILES = $(trans_DATA)
DISTCLEANFILES = $(trans_DATA)
MAINTAINERCLEANFILES = $(trans_DATA)

%.ts: %.qm
	lrelease *.qm
