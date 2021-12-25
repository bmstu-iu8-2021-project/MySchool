QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    sources/addgroupswindow.cpp \
    sources/addlangwindow.cpp \
    sources/addstudentwindow.cpp \
    sources/editwindow.cpp \
    sources/loginwindow.cpp \
    sources/main.cpp \
    sources/mainwindow.cpp

HEADERS += \
    ./headers/addgroupswindow.h \
    ./headers/addlangwindow.h \
    ./headers/addstudentwindow.h \
    ./headers/editwindow.h \
    ./headers/loginwindow.h \
    ./headers/mainwindow.h

FORMS += \
    ui/addgroupswindow.ui \
    ui/addlangwindow.ui \
    ui/addstudentwindow.ui \
    ui/editwindow.ui \
    ui/loginwindow.ui \
    ui/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc
