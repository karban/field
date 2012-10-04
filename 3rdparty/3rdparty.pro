QT -= GUI
TARGET = ../libs/field_3rdparty
OBJECTS_DIR = build
TEMPLATE = lib

contains(CONFIG, HERMES_DEBUG) {
    CONFIG += debug
}

SOURCES += gl2ps/gl2ps.c \
    ctemplate/base/arena.cc \
    ctemplate/htmlparser/htmlparser.cc \
    ctemplate/htmlparser/htmlparser_cpp.h \
    ctemplate/htmlparser/jsparser.cc \
    ctemplate/htmlparser/statemachine.cc \
    ctemplate/per_expand_data.cc \
    ctemplate/template_annotator.cc \
    ctemplate/template_cache.cc \
    ctemplate/template.cc \
    ctemplate/template_dictionary.cc \
    ctemplate/template_modifiers.cc \
    ctemplate/template_namelist.cc \
    ctemplate/template_pathops.cc \
    ctemplate/template_string.cc

linux-g++|linux-g++-64|linux-g++-32 {
    INCLUDEPATH += ctemplate \
            ctemplate/unix \
            .
}

win32-msvc2010 {
	QMAKE_CXXFLAGS += /MP /openmp /Zc:wchar_t
	QMAKE_CXXFLAGS_RELEASE += -MD
    QMAKE_CXXFLAGS_DEBUG += -MDd
	
	LIBS += -Ld:/hpfem/hermes/dependencies/lib
	LIBS += -lblas
    INCLUDEPATH += ctemplate
	INCLUDEPATH += ctemplate/windows

    SOURCES += ctemplate/windows/port.cc
}

macx-g++ {
}
