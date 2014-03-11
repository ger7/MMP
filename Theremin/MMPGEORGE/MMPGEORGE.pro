TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += \
    ../src/utils.c \
    ../src/RealtimePitch.cpp \
    ../src/PortAudioRecPlay.cpp \
    ../src/main.cpp \
    ../src/AudioBuffer.cpp \
    ../src/aubiopitch.c \
    ../src/ArmController.cpp \
    ../cbits/utils.c \
    ../src/getpitch.cpp \
    ../src/notemap.cpp \
    ../src/controller.cpp

OTHER_FILES +=

HEADERS += \
    ../include/utilities.h \
    ../include/parse_args.h \
    ../include/AudioBuffer.h \
    ../include/aubioutils.h \
    ../include/ArmController.h \
    ../include/getpitch.h \
    ../include/controller.h \
    ../include/notemap.h

