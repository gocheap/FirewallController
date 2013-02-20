QT += core gui
	
TEMPLATE = app

TARGET = FirewallController

CONFIG -= embed_manifest_exe

RC_FILE = ../Resources/FWControl.rc

LIBS += \
    ole32.lib \
    oleaut32.lib \
    user32.lib

SOURCES += \
    Main.cpp \
    MainWindow.cpp

HEADERS += \
    MainWindow.hpp
