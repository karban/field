QT += opengl xml network webkit svg xmlpatterns

DEFINES += VERSION_MAJOR=3
DEFINES += VERSION_MINOR=0
DEFINES += VERSION_SUB=0
DEFINES += VERSION_GIT=1971
DEFINES += VERSION_YEAR=2012
DEFINES += VERSION_MONTH=5
DEFINES += VERSION_DAY=27

INCLUDEPATH += ./ \
    ../src \
    ../3rdparty \
    ../3rdparty/ctemplate \
    ../3rdparty/dxflib

linux-g++|linux-g++-64|linux-g++-32 {
    QMAKE_LFLAGS += -fopenmp
    QMAKE_CXXFLAGS += -fopenmp
    # DEFINES += WITH_UNITY

    INCLUDEPATH += /usr/include
    INCLUDEPATH += /usr/include/suitesparse
    INCLUDEPATH += /usr/include/python2.7
    INCLUDEPATH += $$system(python -c "\"import distutils.sysconfig; print distutils.sysconfig.get_python_inc()\"")
    INCLUDEPATH += ../3rdparty/ctemplate/unix

    LIBS += -L../libs

    LIBS += -lumfpack
    LIBS += -lxerces-c
    LIBS += -lamd
    LIBS += -lblas
    LIBS += -lpthread
    LIBS += $$system(python -c "\"from distutils import sysconfig; print '-lpython'+sysconfig.get_config_var('VERSION')\"")
    LIBS += $$system(python -c "\"import distutils.sysconfig; print distutils.sysconfig.get_config_var('LOCALMODLIBS')\"")
    # qwt
    INCLUDEPATH += /usr/include/qwt
    LIBS += -lqwt

    # mumps
    contains(CONFIG, WITH_MUMPS) {
        DEFINES += WITH_MUMPS
        LIBS += -ldmumps_seq
        LIBS += -lzmumps_seq
    }
    # superlu
    contains(CONFIG, WITH_SUPERLU) {
        DEFINES += WITH_SUPERLU
        INCLUDEPATH += /usr/include/superlu
        LIBS += -lsuperlu
    }

    # unity
    contains(CONFIG, WITH_UNITY) {
        DEFINES += WITH_UNITY
        INCLUDEPATH += /usr/include/unity/unity
        INCLUDEPATH += /usr/include/glib-2.0
        INCLUDEPATH += /usr/lib/x86_64-linux-gnu/glib-2.0/include
        INCLUDEPATH += /usr/include/dee-1.0
        INCLUDEPATH += /usr/include/libdbusmenu-0.4
        LIBS += -lunity
    }
}

macx-g++ {
    INCLUDEPATH += /opt/local/include
    INCLUDEPATH += /opt/local/include/ufsparse
    INCLUDEPATH += /Library/Frameworks/Python.framework/Versions/Current/include/python2.7
    INCLUDEPATH += ../../qwt-6.0.1/src

    LIBS += -L/opt/local/lib
    LIBS += -L/usr/lib
    LIBS += -L/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/config
    LIBS += -L../../qwt-6.0.1/lib
    LIBS += -lpthread
    LIBS += -lpython2.7
    LIBS += -lqwt
    LIBS += -lumfpack
    LIBS += -lamd
    LIBS += -lblas
}

win32-msvc2010 {
    QMAKE_CXXFLAGS += /MP /openmp /Zc:wchar_t
	QMAKE_CXXFLAGS_RELEASE += -MD
    QMAKE_CXXFLAGS_DEBUG += -MDd

    #DEFINES += XERCES_STATIC_LIBRARY
    #DEFINES += XML_LIBRARY
    DEFINES += "finite=_finite"
    DEFINES += "popen=_popen"

    INCLUDEPATH += c:/Python27/include
    INCLUDEPATH += ../../qwt-6.0.1/src
    INCLUDEPATH += ../3rdparty/ctemplate/windows
    INCLUDEPATH += d:/hpfem/hermes/dependencies/include

    LIBS += -L../3rdparty/libs
    LIBS += -L../weakform/libs
	LIBS += -Ld:/hpfem/hermes/dependencies/lib

	LIBS += -Lc:/Python27/libs
    LIBS += -L../../qwt-6.0.1/lib
    LIBS += -lvcomp
    LIBS += -lqwtd
    LIBS += -lpython27
    LIBS += -llibumfpack
    LIBS += -llibamd
    LIBS += -lpthreadVCE2
    LIBS += -lxerces-c_static_3D
    LIBS += -ladvapi32
    LIBS += -lws2_32
}
