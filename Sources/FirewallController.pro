QT += core gui
	
TEMPLATE = app

TARGET = FirewallController

release {
    DESTDIR = ../Binary/Release
    OBJECTS_DIR = ../Build/Release
}

debug {
    DESTDIR = ../Binary/Debug
    OBJECTS_DIR = ../Build/Debug
}

CONFIG -= embed_manifest_exe

RC_FILE = ../Resources/FWControl.rc

RESOURCES += \
    ../Resources/Resources.qrc

LIBS += \
    ole32.lib \
    oleaut32.lib

SOURCES += \
    Main.cpp \
    MainWindow.cpp

HEADERS += \
    MainWindow.hpp
