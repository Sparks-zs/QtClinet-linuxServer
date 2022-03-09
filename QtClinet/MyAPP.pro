QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += network widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    filetransfer/filebutton.cpp \
    filetransfer/filetransfer.cpp \
    filetransfer/flowlayout.cpp \
    filetransfer/thread.cpp \
    filetransfer/toolbutton.cpp \
    main.cpp \
    mainwindow/mainwindow.cpp \
    mainwindow/mylayout.cpp \
    serverm/serverm.cpp \
    mainwindow/switchbutton.cpp \
    tcp/httpconn.cpp \
    tcp/tcpsocket.cpp

HEADERS += \
    filetransfer/filebutton.h \
    filetransfer/filetransfer.h \
    filetransfer/flowlayout.h \
    filetransfer/thread.h \
    filetransfer/toolbutton.h \
    mainwindow/mainwindow.h \
    mainwindow/mylayout.h \
    serverm/serverm.h \
    mainwindow/switchbutton.h \
    tcp/httpconn.h \
    tcp/tcpsocket.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

RESOURCES += \
    resources.qrc
    resources/images/file.png
    resources/images/folder.png
    resources/images/loading/loading-0.jpg
    resources/images/loading/loading-1.jpg
    resources/images/loading/loading-2.jpg
    resources/images/loading/loading-3.jpg
    resources/images/loading/loading-4.jpg
    resources/images/loading/loading-5.jpg
    resources/images/loading/loading-6.jpg
    resources/images/loading/loading-7.jpg
    resources/images/loading/loading-8.jpg
    resources/images/loading/loading-9.jpg
    resources/images/loading/loading-10.jpg
    resources/images/loading/loading-11.jpg
    resources/images/loading/loading-12.jpg
    resources/images/loading/loading-13.jpg
    resources/images/loading/loading-14.jpg
    resources/images/loading/loading-15.jpg
    resources/images/loading/loading-16.jpg
    resources/images/loading/loading-17.jpg
    resources/images/loading/loading-18.jpg
    resources/images/loading/loading-19.jpg
    resources/images/loading/loading-20.jpg
    resources/images/loading/loading-21.jpg
    resources/images/loading/loading-22.jpg
    resources/images/loading/loading-23.jpg
    resources/images/loading/loading-24.jpg
    resources/images/loading/loading-25.jpg
    resources/images/loading/loading-26.jpg
    resources/images/loading/loading-27.jpg
    resources/images/loading/loading-28.jpg
    resources/images/loading/loading-29.jpg
    resources/images/loading/loading-30.jpg
    resources/images/loading/loading-31.jpg
    resources/images/loading/loading-32.jpg
    resources/images/loading/loading-33.jpg
    resources/images/loading/loading-34.jpg
    resources/images/loading/loading-35.jpg
    resources/images/loading/loading-36.jpg
    resources/images/loading/loading-37.jpg
    resources/images/loading/loading-38.jpg
    resources/images/loading/loading-39.jpg
    resources/images/loading/loading-40.jpg
    resources/images/loading/loading-41.jpg
    resources/images/loading/loading-42.jpg
    resources/images/loading/loading-43.jpg
    resources/images/loading/loading-44.jpg
    resources/images/loading/loading-45.jpg
    resources/images/loading/loading-46.jpg
    resources/images/loading/loading-47.jpg
    resources/images/loading/loading-48.jpg
    resources/images/loading/loading-49.jpg
    resources/images/loading/loading-50.jpg


