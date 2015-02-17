#-------------------------------------------------
#
# Project created by QtCreator 2015-01-13T11:10:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NESMetaspriteTool
TEMPLATE = app

INCLUDEPATH += widgets/lineedit/ \
	widgets/metasprite/ \
	widgets/palette/ \
	widgets/sprite/ \
	widgets/tileset/


SOURCES += main.cpp \
        nesmetaspritetool.cpp \
	widgets/palette/palettemanager.cpp \
	widgets/sprite/spritepaletteview.cpp \
	widgets/tileset/tilesetmanager.cpp \
	widgets/metasprite/metaspritemanager.cpp \
	widgets/metasprite/metaspritetileitem.cpp \
	widgets/sprite/spritelistwidget.cpp \
    widgets/lineedit/lineeditasm.cpp

HEADERS  += nesmetaspritetool.h \
	widgets/palette/palettemanager.h \
	widgets/sprite/spritepaletteview.h \
	widgets/tileset/tilesetmanager.h \
	widgets/metasprite/metaspritemanager.h \
	widgets/metasprite/metaspritetileitem.h \
	widgets/sprite/spritelistwidget.h \
    widgets/lineedit/lineeditasm.h

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
