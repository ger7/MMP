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
    ../cbits/simpleVector.c

OTHER_FILES +=

HEADERS += \
    ../include/utilities.h \
    ../include/parse_args.h \
    ../include/AudioBuffer.h \
    ../include/aubioutils.h \
    ../include/ArmController.h \
    ../cbits/simpleVector.h

