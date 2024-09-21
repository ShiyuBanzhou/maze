QT       += core gui
QT       += multimedia # QSound

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    imgbutton.cpp \
    main.cpp \
    mainwindow.cpp \
    mazegenerator.cpp \
    playwindow.cpp \
    registerloginwindow.cpp \
    user.cpp

HEADERS += \
    imgbutton.h \
    mainwindow.h \
    mazegenerator.h \
    playwindow.h \
    registerloginwindow.h \
    user.h

FORMS += \
    mainwindow.ui \
    playwindow.ui \
    registerloginwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
