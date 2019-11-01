#-------------------------------------------------
#
# Project created by QtCreator 2017-05-21T14:43:49
#
#-------------------------------------------------

QT       += core gui widgets

TEMPLATE = app

#---------------------------------------------------------------------------------------------------

linux {
}
win32 {
#QT += winextras
}

#---------------------------------------------------------------------------------------------------
# Target
#---------------------------------------------------------------------------------------------------
CONFIG(debug, debug|release) {
TARGET = Finder_d
DEFINES += DEBUG
} else {
TARGET = Finder
}
#---------------------------------------------------------------------------------------------------
# Versión
#---------------------------------------------------------------------------------------------------
DEFINES += MAJOR_VERSION=1
DEFINES += MINOR_VERSION=0

#---------------------------------------------------------------------------------------------------
# Directorio de destino
#---------------------------------------------------------------------------------------------------
DESTDIR = ../bin

#---------------------------------------------------------------------------------------------------
# Archivos de código
#---------------------------------------------------------------------------------------------------

SOURCES += \
		src/main.cpp \
		src/MainWindow.cpp \
    src/AboutDialog.cpp \
    src/Languages.cpp \
    src/Finder.cpp \
    src/FindingDialog.cpp \
    src/utils/IconFromFile.cpp \
    src/utils/ShowInContainerFolder.cpp \
    src/utils/GetFileTypeFromFileInfo.cpp

HEADERS  += \
		src/MainWindow.h \
		src/utils.h \
    src/AboutDialog.h \
    src/Languages.h \
    src/Finder.h \
    src/FindingDialog.h

FORMS    += \
		src/MainWindow.ui \
    src/AboutDialog.ui \
    src/FindingDialog.ui

#---------------------------------------------------------------------------------------------------
# Archivos de recursos
#---------------------------------------------------------------------------------------------------

RESOURCES += res/resource.qrc

win32:RC_FILE = res/resource_win32.rc

#---------------------------------------------------------------------------------------------------
# Archivos de traducción
#---------------------------------------------------------------------------------------------------

TRANSLATIONS =	translate/finder_es.ts translate/finder_en.ts


