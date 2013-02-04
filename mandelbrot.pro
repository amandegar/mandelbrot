QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS       = mandelbrotwidget.h \
                renderthread.h \
    mandelbrotcore.h \
    listenmpi.h \
    argument.h
SOURCES       = main.cpp \
                mandelbrotwidget.cpp \
                renderthread.cpp \
    mandelbrotcore.cpp \
    listenmpi.cpp

unix:!mac:!symbian:!vxworks:LIBS += -lm

# install
target.path = $$[QT_INSTALL_EXAMPLES]/threads/mandelbrot
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS mandelbrot.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/threads/mandelbrot
INSTALLS += target sources

QMAKE_CC = gcc \
    $$system(mpicc --showme:compile)
QMAKE_CXX = g++ \
    $$system(mpic++ --showme:compile)
QMAKE_LFLAGS = $$system(mpic++ --showme:link)

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
