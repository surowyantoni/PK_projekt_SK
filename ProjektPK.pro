QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ARXgenerate.cpp \
    GenW.cpp \
    GenWar.cpp \
    RegulatorOnOff.cpp \
    RegulatorPID.cpp \
    UAR.cpp \
    WarstwaUslug.cpp \
    main.cpp \
    mainwindow.cpp \
    parametryarx.cpp \
    qcustomplot.cpp

HEADERS += \
    ARXgenerate.h \
    GenW.h \
    GenW.h \
    RegulatorOnOff.h \
    RegulatorPID.h \
    UAR.h \
    WarstwaUslug.h \
    WarstwaUslug.h \
    mainwindow.h \
    parametryarx.h \
    qcustomplot.h \
    timercpp.h

FORMS += \
    mainwindow.ui \
    parametryarx.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    wzor.qrc
