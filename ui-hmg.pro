QT       += core gui texttospeech multimedia mqtt
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = ui-hmg
TEMPLATE = app

HEADERS += $$files(includes/*.h, true)
           
LIBS += -L$$PWD/lib -lvosk
LIBS += -L"E:/vosk-model-cn/OpenBLAS-0.3.29_x64/lib" -lopenblas

SOURCES += $$files(src/*.cpp, true)

TRANSLATIONS += ui-hmg_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

FORMS +=
