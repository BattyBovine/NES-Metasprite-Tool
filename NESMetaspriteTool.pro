#-------------------------------------------------
#
# Project created by QtCreator 2015-01-13T11:10:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NESMetaspriteTool
TEMPLATE = app

INCLUDEPATH += widgets/ui \
	widgets/metasprite \
	widgets/palette \
	widgets/sprite \
	widgets/tileset \
	widgets/animation


SOURCES += main.cpp \
	nesmetaspritetool.cpp \
	widgets/ui/lineeditasm.cpp \
	widgets/ui/graphicsviewslider.cpp \
	widgets/ui/spritelistwidget.cpp \
	widgets/ui/framelistwidget.cpp \
	widgets/palette/palettemanager.cpp \
	widgets/sprite/spritepaletteview.cpp \
	widgets/tileset/tilesetmanager.cpp \
	widgets/metasprite/metaspritemanager.cpp \
	widgets/metasprite/metaspritetileitem.cpp \
	widgets/animation/animationmanager.cpp \
	widgets/animation/animationframemanager.cpp \
	widgets/animation/animationframeitem.cpp \
    widgets/tileset/chrthread.cpp \
    widgets/tileset/tilesetcache.cpp \
    widgets/tileset/globaltilesetmanager.cpp

HEADERS  += nesmetaspritetool.h \
	widgets/ui/lineeditasm.h \
	widgets/ui/graphicsviewslider.h \
	widgets/ui/spritelistwidget.h \
	widgets/ui/framelistwidget.h \
	widgets/palette/palettemanager.h \
	widgets/sprite/spritepaletteview.h \
	widgets/tileset/tilesetmanager.h \
	widgets/metasprite/metaspritemanager.h \
	widgets/metasprite/metaspritetileitem.h \
	widgets/animation/animationmanager.h \
	widgets/animation/animationframemanager.h \
	widgets/animation/animationframeitem.h \
    widgets/tileset/chrthread.h \
    widgets/tileset/tilesetcache.h \
    widgets/tileset/globaltilesetmanager.h

FORMS    += nesmetaspritetool.ui

RESOURCES += \
    res/chr.qrc \
    res/palettes.qrc


win32:RC_ICONS += res/icon.ico
ICON = res/icon.icns

QMAKE_TARGET_PRODUCT = "NES Metasprite Tool"
QMAKE_TARGET_COMPANY = "Batty Bovine Productions, LLC"
QMAKE_TARGET_COPYRIGHT = "(c) 2015 Batty Bovine Productions, LLC. Some Rights Reserved."
GENERATED_VERSION_NUMBER = $$system(perl versionup.pl -get)
VERSION = $${GENERATED_VERSION_NUMBER}
