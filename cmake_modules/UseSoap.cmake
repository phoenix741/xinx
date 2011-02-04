set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTXML TRUE)

set(SOAP_PATH ${CMAKE_SOURCE_DIR}/ext/qtsoap/src)

include_directories(${SOAP_PATH})

set(SOAP_SOURCES ${SOAP_PATH}/qtsoap.cpp)
set(SOAP_HEADERS ${SOAP_PATH}/qtsoap.h)
