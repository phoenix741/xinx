
INCLUDEPATH += $$IDEALITY_DIR

CONFIG += release

contains(CONFIG, debug_and_release_target) {
    CONFIG(debug, debug|release) {
        LIBS+=-L$$IDEALITY_DIR/debug
    } else {
        LIBS+=-L$$IDEALITY_DIR/release
    }
} else {
    LIBS += -L$$IDEALITY_DIR
}


hpux-acc* {
	LIBS += $$IDEALITY_DIR/libideality.a
} else {
	LIBS += -lideality
}


