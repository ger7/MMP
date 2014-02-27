# Created by Patricia Shaw, Aberystwyth University, Feb 2012

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(iCubTheremin)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_MODULE_PATH $(CMAKE_MODULE_PATH))

FIND_PACKAGE(YARP REQUIRED)

INCLUDE_DIRECTORIES(
	../portaudio/bindings/cpp/include/
	../portaudio/include/
	../portaudio/src/common/
	../aubio-0.4.0/src
	${PROJECT_SOURCE_DIR}/include
	${YARP_INCLUDE_DIRS}
)
LINK_DIRECTORIES(
	${PROJECT_SOURCE_DIR}../aubio-0.4.0/build/src
	../cbits/
)

FILE(GLOB c_code
	src/utils.c
	include/aubioutils.h
	include/parse_args.h
)

#ADD_LIBRARY(aubioclib ${c_code})
#TARGET_LINK_LIBRARIES(aubioclib)


FILE(GLOB folder_source 
#	src/ArmController.cpp
	
	src/AudioBuffer.cpp
	
)
FILE(GLOB folder_header
	include/ArmController.h
	include/AudioBuffer.h
	include/utilities.h
)

SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})


#ADD_EXECUTABLE(main ${PROJECT_SOURCE_DIR}/src/main.cpp ${folder_header} ${folder_source})
ADD_EXECUTABLE(RealtimePitch ${PROJECT_SOURCE_DIR}/src/RealtimePitch.cpp ${folder_header} ${folder_source})

#TARGET_LINK_LIBRARIES(main ${YARP_LIBRARIES})
TARGET_LINK_LIBRARIES(RealtimePitch  portaudiocpp pthread portaudio Caubio aubio)
