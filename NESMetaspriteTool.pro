#-------------------------------------------------
#
# Project created by QtCreator 2015-01-13T11:10:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NESMetaspriteTool
TEMPLATE = app


SOURCES += main.cpp\
        nesmetaspritetool.cpp \
    palettemanager.cpp \
    spritepaletteview.cpp \
    tilesetmanager.cpp \
    metaspritemanager.cpp

HEADERS  += nesmetaspritetool.h \
    palettemanager.h \
    spritepaletteview.h \
    tilesetmanager.h \
    metaspritemanager.h

FORMS    += nesmetaspritetool.ui

RESOURCES += \
    palettes.qrc


win32:RC_ICONS += res/icon.ico
ICON = res/icon.icns
