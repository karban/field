SUBDIRS += 3rdparty
SUBDIRS += src
SUBDIRS += binary

CONFIG += ordered
TEMPLATE = subdirs

linux-g++ {
    # use qmake PREFIX=... to customize your installation
    isEmpty(PREFIX):PREFIX = /usr/local

    # install examples
    examples.path = $${PREFIX}/share/field/data
    examples.files = data/*.fld

    # install script
    script.path = $${PREFIX}/share/field/data/script
    script.files = data/script/*.py

    # install help
    help.path = $${PREFIX}/share/field/resources/help
    help.files = resources/help/*

    # install report
    report.path = $${PREFIX}/share/field/doc/report
    report.files = doc/report/*

    # install translations
    lang.path = $${PREFIX}/share/field/lang
    lang.files = lang/*.qm

    # install script
    script.path = $${PREFIX}/share/field
    script.files = *.py

    # install pixmap
    pixmap.path = $${PREFIX}/share/pixmaps
    pixmap.files = src/images/field.xpm

    # install desktop
    desktop.path = $${PREFIX}/share/applications
    desktop.files = field.desktop

    # install binary
    system(touch field)
    target.path = $${PREFIX}/bin
    target.files = field
    
    # install libraries
    script.path = $${PREFIX}/lib/
    script.files = libs/*

    # "make install" configuration options
    INSTALLS *= target \
        target-remote \
        examples \
        help \
        lang \
        script \
        pixmap \
        report \
        desktop
}

OTHER_FILES += \
    field.supp


