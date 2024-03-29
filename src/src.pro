# agros2d - hp-FEM multiphysics application based on Hermes2D library

TARGET = ../libs/field
OBJECTS_DIR = build
MOC_DIR = build

TEMPLATE = lib

# backup
# VERSION_GIT=$$system(git log --pretty=format:%h | wc -l)
# run cython for python extensions
linux-g++|linux-g++-64|linux-g++-32 : CONFIG(release) {
    system(cython --cplus ../resources_source/python/field.pyx)
    system(cd ../ && ./field.sh lang release)
    #system(cd ../ && ./field.sh help)
}

TRANSLATIONS = lang/cs_CZ.ts \
    lang/pl_PL.ts \
    lang/de_DE.ts
CODECFORTR = UTF-8
RC_FILE = src.rc
RESOURCES = src.qrc

SOURCES += util.cpp \
    value.cpp \
    scene.cpp \
    util/checkversion.cpp \
    util/point.cpp \
    util/xml.cpp \
    gui/common.cpp \
    gui/chart.cpp \
    gui/filebrowser.cpp \
    gui/imageloader.cpp \
    gui/about.cpp \
    gui/lineeditdouble.cpp \
    gui/htmledit.cpp \
    gui/textedit.cpp \
    gui/systemoutput.cpp \
    pythonlab/pythonconsole.cpp \
    pythonlab/pythoncompleter.cpp \
    pythonlab/pythonhighlighter.cpp \
    pythonlab/pythonengine.cpp \
    pythonlab/pythonbrowser.cpp \
    pythonlab/pythoneditor.cpp \
    pythonlabagros.cpp \
    mainwindow.cpp \
    scenebasic.cpp \
    # sceneview.cpp \
    sceneview_common.cpp \
    sceneview_common3d.cpp \
    sceneview_geometry.cpp \
    sceneview_post.cpp \
    sceneview_post3d.cpp \
    chartdialog.cpp \    
    field/problem.cpp \
    problemdialog.cpp \
    scenetransformdialog.cpp \
    tooltipview.cpp \
    logview.cpp \
    postprocessorview.cpp \
    style/stylehelper.cpp \
    style/styleanimator.cpp \
    style/manhattanstyle.cpp \
    indicators/indicators.cpp \
    indicators/indicator_unity.cpp \
    resultsview.cpp \
    conf.cpp \
    confdialog.cpp \
    scenenode.cpp \
    preprocessorview.cpp \
    infowidget.cpp \
    settings.cpp \
    parser/lex.cpp \
    gui/groupbox.cpp

HEADERS += util.h \
    value.h \
    scene.h \
    util/constants.h \
    util/checkversion.h \
    util/point.h \
    util/xml.h \
    gui/common.h \
    gui/chart.h \
    gui/filebrowser.h \
    gui/imageloader.h \
    gui/about.h \
    gui/lineeditdouble.h \
    gui/htmledit.h \
    gui/textedit.h \
    gui/systemoutput.h \
    pythonlab/pythonconsole.h \
    pythonlab/pythoncompleter.h \
    pythonlab/pythonhighlighter.h \
    pythonlab/pythonengine.h \
    pythonlab/pythonbrowser.h \
    pythonlab/pythoneditor.h \
    pythonlabagros.h \
    mainwindow.h \
    scenebasic.h \
    scenenode.h \
    sceneview_data.h \
    sceneview_common.h \
    sceneview_common3d.h \
    sceneview_geometry.h \
    sceneview_post.h \
    sceneview_post3d.h \
    meshgenerator.h \
    meshgenerator_triangle.h \
    meshgenerator_gmsh.h \
    chartdialog.h \
    field/problem.h \
    problemdialog.h \
    scenetransformdialog.h \
    reportdialog.h \
    tooltipview.h \
    logview.h \
    postprocessorview.h \
    style/stylehelper.h \
    style/styleanimator.h \
    style/manhattanstyle.h \
    indicators/indicators.h \
    indicators/indicator_unity.h \
    resultsview.h \
    conf.h \
    confdialog.h \
    preprocessorview.h \
    infowidget.h \
    settings.h \
    parser/lex.h \
    gui/groupbox.h

OTHER_FILES += python/field.pyx \
    functions.py \
    version.xml \
    ../resources/modules/rf.xml \
    ../resources/modules/magnetic.xml \
    ../resources/modules/heat.xml \
    ../resources/modules/electrostatic.xml \
    ../resources/modules/elasticity.xml \
    ../resources/modules/current.xml \
    ../resources/modules/acoustic.xml \
    ../resources/modules/flow.xml \
    ../resources/couplings/heat-elasticity.xml \
    ../resources/couplings/current-heat.xml \
    ../resources/couplings/magnetic-heat.xml \
    ../resources/couplings/flow-heat.xml \
    ../resources/modules/rf.xml \
    ../resources_source/python/field.pyx \
    ../resources/xsd/subdomains_h2d_xml.xsd \
    ../resources/xsd/module_xml.xsd \
    ../resources/xsd/mesh_h2d_xml.xsd \
    ../resources/xsd/coupling_xml.xsd

LIBS += -lfield_3rdparty

linux-g++|linux-g++-64|linux-g++-32 {
    # DEFINES += WITH_UNITY

    QMAKE_CXXFLAGS += -Wno-unused-variable
}

macx-g++ {
    ICON += resources/images/field.icns
    QMAKE_INFO_PLIST  += resources/Info.plist
    #target.path = /Applications
    #INSTALLS += target

    INCLUDEPATH += /opt/local/include
    INCLUDEPATH += /opt/local/include/ufsparse
    INCLUDEPATH += /Library/Frameworks/Python.framework/Versions/Current/include/python2.7
    INCLUDEPATH += ../../qwt-6.0.1/src

    LIBS += -L/opt/local/lib
    LIBS += -L/usr/lib
    LIBS += -L/Library/Frameworks/Python.framework/Versions/2.7/3rdparty/python2.7/config
    LIBS += -L../../qwt-6.0.1/lib
    LIBS += -lpthread
    LIBS += -lpython2.7
    LIBS += -lqwt
    LIBS += -lumfpack
    LIBS += -lamd
    LIBS += -lblas
}

win32-msvc2010 {
    CONFIG += staticlib
    QMAKE_CXXFLAGS += /MP /openmp /Zc:wchar_t
	QMAKE_CXXFLAGS_RELEASE += -MD
    QMAKE_CXXFLAGS_DEBUG += -MDd

    #DEFINES += XERCES_STATIC_LIBRARY
	DEFINES += AGROS
    #DEFINES += XML_LIBRARY
    DEFINES += "finite=_finite"
    DEFINES += "popen=_popen"

    INCLUDEPATH += c:/Python27/include
    INCLUDEPATH += ../../qwt-6.0.1/src
    INCLUDEPATH += ../3rdparty/ctemplate/windows
    INCLUDEPATH += d:/hpfem/hermes/dependencies/include
}

include(../field.pri)
