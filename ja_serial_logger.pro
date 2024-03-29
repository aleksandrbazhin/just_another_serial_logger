#-------------------------------------------------
#
# Project created by QtCreator 2019-05-29T21:59:43
#
#-------------------------------------------------

QT       += core gui serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ja_serial_logger
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
RC_FILE = ja_serial_logger.rc

SOURCES += \
        datachart.cpp \
        main.cpp \
        mainwindow.cpp \
        settingsdialog.cpp \
        thirdparty/qcustomplot.cpp

HEADERS += \
        datachart.h \
        mainwindow.h \
        portscombobox.h \
        settingsdialog.h \
        thirdparty/qcustomplot.h

FORMS += \
        mainwindow.ui \
        settingsdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resourses.qrc

DISTFILES += \
    images/drag-icon.png \
    ja_serial_logger.rc \
    jasl.ico
