QMAKE_EXTRA_TARGETS += before_build makefilehook
makefilehook.target = $(MAKEFILE)
makefilehook.depends = .beforebuild

PRE_TARGETDEPS += .beforebuild
before_build.target = .beforebuild
before_build.depends = FORCE
before_build.commands = chcp 1251

QT += core gui opengl

CONFIG += c++11 console
CONFIG -= app_bundle

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
        AppWindow.cpp \
        ArrangementWidget.cpp \
        ComparisonSidesWidget.cpp \
        CoordinateSystem.cpp \
        Grid.cpp \
        HeightMatrix.cpp \
        HeightMatrixIterator.cpp \
        MatrixWidget.cpp \
        TargetMatrixWidget.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    AppWindow.ui

HEADERS += \
    AppWindow.h \
    ArrangementWidget.h \
    ComparisonSidesWidget.h \
    CoordinateSystem.h \
    Grid.h \
    HeightMatrix.h \
    HeightMatrixIterator.h \
    MatrixWidget.h \
    TargetMatrixWidget.h

RESOURCES += \
    Shaders.qrc

DISTFILES += \
    todoList.txt
