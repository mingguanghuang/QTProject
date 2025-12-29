QT       += core gui texttospeech multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = ui-hmg
TEMPLATE = app


HEADERS += \
           widget.h \
           ledpage.h \
           voicecontrolpage.h \
           voicesettingpage.h \
           deviceaddpage.h \
           networksettingpage.h \
           securitysettingpage.h

SOURCES += \
           main.cpp \
           widget.cpp \
           ledpage.cpp \
           voicecontrolpage.cpp \
           voicesettingpage.cpp \
           deviceaddpage.cpp \
           networksettingpage.cpp \
           securitysettingpage.cpp

TRANSLATIONS += ui-hmg_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

FORMS +=
