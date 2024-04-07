QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    LoginWidget.cpp \
    addfriend.cpp \
    addgroup.cpp \
    application.cpp \
    creategroup.cpp \
    groupchat.cpp \
    main.cpp \
    mainwidget.cpp \
    privatechat.cpp \
    recvfile.cpp \
    sendthread.cpp \
    tcp.cpp

HEADERS += \
    LoginWidget.h \
    addfriend.h \
    addgroup.h \
    application.h \
    creategroup.h \
    groupchat.h \
    mainwidget.h \
    privatechat.h \
    recvfile.h \
    sendthread.h \
    tcp.h

FORMS += \
    LoginWidget.ui \
    addfriend.ui \
    addgroup.ui \
    application.ui \
    creategroup.ui \
    groupchat.ui \
    mainwidget.ui \
    privatechat.ui

LIBS += -lws2_32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
