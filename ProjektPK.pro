QT       += core gui charts network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ARXgenerate.cpp \
    GenW.cpp \
    RegulatorOnOff.cpp \
    RegulatorPID.cpp \
    UAR.cpp \
    WarstwaUslug.cpp \
    client.cpp \
    connectionwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    netservice.cpp \
    parametryarx.cpp \
    qcustomplot.cpp \
    server.cpp

HEADERS += \
    ARXgenerate.h \
    GenW.h \
    GenW.h \
    ProtocolDef.h \
    RegulatorOnOff.h \
    RegulatorPID.h \
    UAR.h \
    WarstwaUslug.h \
    WarstwaUslug.h \
    client.h \
    connectionwindow.h \
    mainwindow.h \
    netservice.h \
    parametryarx.h \
    qcustomplot.h \
    server.h \
    timercpp.h

FORMS += \
    connectionwindow.ui \
    mainwindow.ui \
    parametryarx.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    wzor.qrc
