#-------------------------------------------------
#
# Project created by QtCreator 2015-01-13T11:10:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NESMetaspriteTool
TEMPLATE = app


SOURCES += main.cpp \
        nesmetaspritetool.cpp \
    palettemanager.cpp \
    spritepaletteview.cpp \
    tilesetmanager.cpp \
    metaspritemanager.cpp \
    metaspritetileitem.cpp

HEADERS  += nesmetaspritetool.h \
    palettemanager.h \
    spritepaletteview.h \
    tilesetmanager.h \
    metaspritemanager.h \
    metaspritetileitem.h

FORMS    += nesmetaspritetool.ui

RESOURCES += \
    palettes.qrc


win32:RC_ICONS += res/icon.ico
ICON = res/icon.icns

QMAKE_TARGET_PRODUCT = "NES Metasprite Tool"
QMAKE_TARGET_COMPANY = "Batty Bovine Productions, LLC"
QMAKE_TARGET_COPYRIGHT = "(c) 2015 Batty Bovine Productions, LLC. Some Rights Reserved."
GENERATED_VERSION_NUMBER = $$system(perl versionup.pl -get)
VERSION = $${GENERATED_VERSION_NUMBER}
