QT       += core gui multimedia sql
QT += multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    playervideos.cpp \
    playlist.cpp \
    removing.cpp \
    stylesloader.cpp \
    themes.cpp \
    viewplaylists.cpp

HEADERS += \
    playervideos.h \
    playlist.h \
    removing.h \
    stylesloader.h \
    themes.h \
    viewplaylists.h

FORMS += \
    playervideos.ui \
    playlist.ui \
    removing.ui \
    themes.ui \
    viewplaylists.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon.qrc \
    nodes.qrc \
    styles.qrc
