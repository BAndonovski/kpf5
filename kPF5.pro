CONFIG(debug, debug|release) DIR = debug
CONFIG(release, debug|release) DIR = release
DESTDIR = bin/$$DIR
OBJECTS_DIR = build/$$DIR
MOC_DIR = src/moc
UI_DIR = src/ui
RCC_DIR = res
VERSION = 1.0.1
INCLUDEPATH += include \
    src/ui \
    src/moc \
    ext \
    ext/libserial
#LIBS += -lserial
QT -= gui
TARGET = kPF5
TEMPLATE = lib
DEFINES += KPF5_LIBRARY
SOURCES += src/KPf5Printer.cpp
HEADERS += include/kPF5_global.h \
    include/KPf5Printer.h \
    include/KPF5_Data.h
macx{
    SOURCES += ext/libserial/SerialStreamBuf.cc \
    ext/libserial/SerialStream.cc \
    ext/libserial/SerialPort.cpp \
    ext/libserial/PosixSignalDispatcher.cpp
    HEADERS += ext/libserial/SerialStreamBuf.h \
    ext/libserial/SerialStream.h \
    ext/libserial/SerialPort.h \
    ext/libserial/PosixSignalHandler.h \
    ext/libserial/PosixSignalDispatcher.h
}
