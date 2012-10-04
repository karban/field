# agros2d - hp-FEM multiphysics application based on Hermes2D library
TARGET = ../field
OBJECTS_DIR = build
MOC_DIR = build
TEMPLATE = app

LIBS += -lfield_3rdparty
LIBS += -lfield

RC_FILE = ../src/src.rc
RESOURCES = ../src/src.qrc

SOURCES += main.cpp

macx-g++ {
    ICON += resources/images/field.icns
    QMAKE_INFO_PLIST  += resources/Info.plist
    #target.path = /Applications
    #INSTALLS += target
}

include(../field.pri)
