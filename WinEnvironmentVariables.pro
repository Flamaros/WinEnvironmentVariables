#-------------------------------------------------
#
# Project created by QtCreator 2014-05-01T18:57:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WinEnvironmentVariables
TEMPLATE = app

SOURCES += \
    main.cpp \
    Environment.cpp \
	MainWindow.cpp \
	Registry.cpp

HEADERS  += \
    Environment.h \
    MainWindow.h \
	Registry.h

FORMS    += MainWindow.ui

QMAKE_CXXFLAGS += -std=c++0x
