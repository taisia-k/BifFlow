QT += core gui widgets

CONFIG += c++17

TARGET = BidFlow
TEMPLATE = app

SOURCES += main.cpp

HEADERS += \
    include/models.h \
    include/appstate.h \
    include/theme.h \
    include/widgets.h \
    include/authwindow.h \
    include/chatwidget.h \
    include/lotdialog.h \
    include/createlotdialog.h \
    include/mainwindow.h
