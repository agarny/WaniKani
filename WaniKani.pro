ARGET = WaniKani

TEMPLATE = app

QT += core gui network widgets

DEFINES += QT_DEPRECATED_WARNINGS

win32: LIBS += -lUser32
mac: LIBS += -framework AppKit

INCLUDEPATH += src/3rdparty/QtSingleApplication

SOURCES = src/main.cpp \
          src/wanikaniwidget.cpp \
          src/3rdparty/QtSingleApplication/qtlocalpeer.cpp \
          src/3rdparty/QtSingleApplication/qtsingleapplication.cpp

HEADERS = src/wanikaniwidget.h \
          src/3rdparty/QtSingleApplication/qtlocalpeer.h \
          src/3rdparty/QtSingleApplication/qtsingleapplication.h

FORMS = src/wanikaniwidget.ui

mac: OBJECTIVE_SOURCES = src/macos.mm

RESOURCES = res/resources.qrc

mac: QMAKE_INFO_PLIST = Res/Info.plist
