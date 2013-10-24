
#
#Some important notes. For a list of dependencies see README.txt.
#This file:
#
#	*find dependencies using the FIND_PACKAGE macro. See /CMakeModules/
# 		for a description of how each package is found.
#
#	*recursively searches subdirectories for source and headers files,
#		it does NOT need to be updated if new source files are added.
#
#To use this file:
#
#	(Out of source build) $: ccmake ../iglu-build-desktop ./		
#	(Or in source build)  $: ccmake ./
#
# -Ethan Kerzner (May 2012)

# Require a certain version of Cmake
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.3 FATAL_ERROR)

# Name the project
PROJECT(iglu)

# Add our moudules directory places cmake looks for modules
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

# Where does glew live?
FIND_PACKAGE(GLEW)
INCLUDE_DIRECTORIES( ${GLEW_INCLUDE_DIR} )

# Where does fltk live?
FIND_PACKAGE(FLTK)
INCLUDE_DIRECTORIES( ${FLTK_INCLUDE_DIR} )

# Ask if compiling with FFmpeg to support video textures
OPTION ( HAS_FFMPEG "Use mmpeg?" OFF) 

# The macro HAS_FFMPEG is also definied in the .cpp files. 
IF(HAS_FFMPEG)

	# Where does FFmpeg live?
	FIND_PACKAGE(FFmpeg)
	
	# Include all the libraries of FFmpeg
	INCLUDE_DIRECTORIES( ${FFMPEG_INCLUDE_DIR} )
	INCLUDE_DIRECTORIES( ${FFMPEG_LIBAVFORMAT_INCLUDE_DIR} )
	INCLUDE_DIRECTORIES( ${FFMPEG_LIBAVCODEC_INCLUDE_DIR} )
	INCLUDE_DIRECTORIES( ${FFMPEG_LIBSWSCALE_INCLUDE_DIR} )
	INCLUDE_DIRECTORIES( ${FFMPEG_LIBAVUTIL_INCLUDE_DIR} ) 

	# FFmpeg uses c99 definitions so tell the compiler to use them
	SET(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS") 

ENDIF()


#Where do we look for included files from iglu?
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/iglu )
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR} )

#Recursively find source files matching the regex *.cpp
FILE(GLOB_RECURSE SOURCES "*.cpp" "*.c")

#Recursively find header  files matching the regex *.h
FILE(GLOB_RECURSE HEADERS "*.h")

#Place our finished library in /lib
SET(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")

#Finally, add the library
ADD_LIBRARY(iglu ${SOURCES} ${HEADERS})

INSTALL(TARGETS iglu ARCHIVE DESTINATION lib)
INSTALL(FILES ${PROJECT_SOURCE_DIR}/iglu.h DESTINATION include)

FOREACH(DIR iglu iglu/buffers iglu/consts iglu/errors iglu/glstate 
	    iglu/helpers iglu/interactors iglu/interface iglu/models 
	    iglu/parsing iglu/sampling iglu/variables iglu/vectors iglu/window)

	    set(PATTERN ${PROJECT_SOURCE_DIR}/${DIR}/*.h)

	    FILE(GLOB IGLU_HDRS ${PATTERN})

	    INSTALL(FILES ${IGLU_HDRS} DESTINATION include/${DIR})
ENDFOREACH(DIR)
