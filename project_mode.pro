CONFIG += debug
unix { 
    QMAKE_CC = "ccache gcc"
    QMAKE_CXX = "ccache gcc"
    QMAKE_LFLAGS = -rdynamic
}
MOC_DIR += ./
OBJECTS_DIR += ./
RCC_DIR += ./
QMAKE_CXXFLAGS_RELEASE = "-Os"
DEFINES += NO_QT_PLAINTEXT
