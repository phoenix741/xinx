set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt4 REQUIRED)
find_package(QCodeEdit REQUIRED)
find_package(Ideality REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(LibXslt REQUIRED)
find_package(Mercurial)

if(MERCURIAL_FOUND)
	MERCURIAL_HG_INFO(${PROJECT_SOURCE_DIR} XINX_FRAMEWORK)
endif(MERCURIAL_FOUND)

if(NOT ${XINX_FRAMEWORK_HG_CHANGESET} STREQUAL "")
	string(REPLACE "-" "" XINX_FRAMEWORK_HG_DATE_ISO ${XINX_FRAMEWORK_HG_DATE})
#	La date est positonné sur 5 caractère pour être inférieur à 65536 (on aura un problème en 2017,
#	65526 est la limite max, pour les fichiers de type .rc sous windows pour un numéro de version (w.x.y.z)
	string(SUBSTRING ${XINX_FRAMEWORK_HG_DATE_ISO} 3 5 XINX_FRAMEWORK_HG_DATE_STR)
endif()

configure_file(core/version.h.in core/version.h)

set(USE_COMPONENTS True)
set(QT_USE_QTXML TRUE)
set(QT_USE_QTXMLPATTERNS TRUE)
set(QT_USE_QTSQL TRUE)
set(QT_USE_QTSCRIPT TRUE)

include(UseUniqueApplication)
include(UseXinx)

add_definitions(-D_LIB_BUILD_)

add_xinx_librairie(framework "" 0.10.0.0)
