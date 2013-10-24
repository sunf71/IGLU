# Locate GLEW
# This module defines
# GLEW_LIBRARY
# GLEW_FOUND, if false, do not try to link to gdal 
# GLEW_INCLUDE_DIR, where to find the headers
#
# $GLEW_DIR is an environment variable that would
# correspond to the ./configure --prefix=$GLEW_DIR
#

FIND_PATH(GLEW_INCLUDE_DIR GL/glew.h GL/GLEW.h
    ${GLEW_DIR}/include
    $ENV{GLEW_DIR}/include
    $ENV{GLEW_DIR}
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    /usr/freeware/include
)

FIND_LIBRARY(GLEW_LIBRARY
        NAMES glew GLEW
        PATHS
        ${GLEW_DIR}/lib
        ${GLEW_DIR}/lib64
        $ENV{GLEW_DIR}/lib
        $ENV{GLEW_DIR}/lib64
        $ENV{GLEW_DIR}
        ~/Library/Frameworks
        /Library/Frameworks
        /usr/local/lib
        /usr/lib
        /sw/lib
        /opt/local/lib
        /opt/csw/lib
        /opt/lib
        /usr/freeware/lib64
    )

SET(GLEW_FOUND "NO")
IF(GLEW_LIBRARY AND GLEW_INCLUDE_DIR)
    SET(GLEW_FOUND "YES")
ENDIF(GLEW_LIBRARY AND GLEW_INCLUDE_DIR)
