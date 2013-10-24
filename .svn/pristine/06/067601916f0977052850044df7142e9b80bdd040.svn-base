# Locate IGLU
# This module defines
# IGLU_LIBRARY
# IGLU_FOUND, if false, do not try to link to gdal 
# IGLU_INCLUDE_DIR, where to find the headers
#
# $IGLU_DIR is an environment variable that would
# correspond to the ./configure --prefix=$IGLU_DIR
#

FIND_PATH(IGLU_INCLUDE_DIR iglu.h 
    ${IGLU_DIR}/include
    $ENV{IGLU_DIR}/include
    $ENV{IGLU_DIR}
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

FIND_LIBRARY(IGLU_LIBRARY
        NAMES iglu IGLU
        PATHS
        ${IGLU_DIR}/lib
        ${IGLU_DIR}/lib64
        $ENV{IGLU_DIR}/lib
        $ENV{IGLU_DIR}/lib64
        $ENV{IGLU_DIR}
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

SET(IGLU_FOUND "NO")
IF(IGLU_LIBRARY AND IGLU_INCLUDE_DIR)
    SET(IGLU_FOUND "YES")
ENDIF(IGLU_LIBRARY AND IGLU_INCLUDE_DIR)
