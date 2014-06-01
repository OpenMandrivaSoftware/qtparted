file(GLOB TRANSLATIONS *.ts)
file(GLOB TO_BE_TRANSLATED ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*.h ${CMAKE_SOURCE_DIR}/src/*.ui)
if(UPDATE_TRANSLATIONS)
	qt5_create_translation(QM_FILES SOURCES ${TO_BE_TRANSLATED} ${TRANSLATIONS})
else()
	qt5_add_translation(QM_FILES ${TRANSLATIONS})
endif()

add_custom_target(all DEPENDS ${QM_FILES})

install(FILES ${QM_FILES} DESTINATION share/qtparted/locale)
