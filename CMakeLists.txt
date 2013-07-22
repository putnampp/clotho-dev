cmake_minimum_required (VERSION 2.8)
PROJECT( POPSIM CXX )

SET (POPSIM_LIB_CORENAME "popsim")

SET (POPSIM_VER_MAJ 0)
SET (POPSIM_VER_MIN 1)
SET (POPSIM_VER_RELEASE 0 )

SET (POPSIM_RESOURCES_DIR ${POPSIM_SOURCE_DIR}/cmake)
SET (POPSIM_SRC_DIR       ${POPSIM_SOURCE_DIR}/src)
SET( POPSIM_INCLUDE_DIR   ${POPSIM_SOURCE_DIR}/include )

SET (CMAKE_INSTALL_PREFIX "./popsim" CACHE PATH "Install path prefix, prepended onto install directories")

IF (NOT POPSIM_INSTALL_BIN_DIR )
    SET (POPSIM_INSTALL_BIN_DIR bin )
ENDIF (NOT POPSIM_INSTALL_BIN_DIR )

IF (NOT POPSIM_INSTALL_LIB_DIR )
    SET (POPSIM_INSTALL_LIB_DIR lib )
ENDIF (NOT POPSIM_INSTALL_LIB_DIR )

IF (NOT POPSIM_INSTALL_INCLUDE_DIR )
    SET(POPSIM_INSTALL_INCLUDE_DIR include)
ENDIF (NOT POPSIM_INSTALL_INCLUDE_DIR )

################################################################################
SET ( POPSIM_PACKAGE "popsim" )
SET ( POPSIM_PACKAGE_NAME "POPSIM" )
SET (POPSIM_PACKAGE_VERSION "${POPSIM_VER_MAJ}.${POPSIM_VER_MIN}.${POPSIM_VER_RELEASE}")
SET (POPSIM_PACKAGE_VER_MAJOR "${POPSIM_VER_MAJ}.${POPSIM_VER_MIN}")
SET (POPSIM_PACKAGE_VER_MINOR "${POPSIM_VER_RELEASE}")
SET (POPSIM_PACKAGE_STRING "${POPSIM_PACKAGE_NAME} ${POPSIM_PACKAGE_VERSION}")
SET (POPSIM_PACKAGE_BUGREPORT "putnampp@gmail.com")

MESSAGE( STATUS ${POPSIM_PACKAGE_STRING} )

IF (NOT POPSIM_EXTERNALLY_CONFIGURED )
    SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY 
        ${PROJECT_BINARY_DIR}/bin CACHE PATH "Executable directory")
    SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/lib CACHE PATH "Libraries directory")
    SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/lib CACHE PATH "Static Libraries directory")
ELSE (NOT POPSIM_EXTERNALLY_CONFIGURED )
    IF (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY )
        SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH} )
    ENDIF (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY )
ENDIF (NOT POPSIM_EXTERNALLY_CONFIGURED )

################################################################################
FIND_PACKAGE( Boost COMPONENTS program_options filesystem system thread unit_test_framework REQUIRED )
IF( Boost_FOUND )
    INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
    MESSAGE (STATUS "Boost Include: " ${Boost_INCLUDE_DIR} )
    MESSAGE (STATUS "Boost Library: " ${Boost_LIBRARIES} )
ENDIF( Boost_FOUND )

#FIND_PACKAGE( HDF5 COMPONENTS C CXX REQUIRED )
#FIND_PACKAGE( HDF5 REQUIRED )
#IF( HDF5_FOUND )
#    INCLUDE_DIRECTORIES( ${HDF5_INCLUDE_DIR} )
#    MESSAGE (STATUS "HDF5 Include: " ${HDF5_INCLUDE_DIR} )
#    MESSAGE (STATUS "HDF5 Library: " ${HDF5_LIBRARIES} )
#ENDIF( HDF5_FOUND )

#SET( MongoDB_INCLUDE_DIR /opt/mongo/include )
#SET( MongoDB_LIBRARIES /opt/mongo/lib/libmongoclient.a )

#MESSAGE( STATUS "MongoDB: " ${MongoDB_INCLUDE_DIR} )
#MESSAGE( STATUS "MongoDB libs: " ${MongoDB_LIBRARIES} )

################################################################################

OPTION( LOG       "Compile with debugging; sets LOG_LEVEL to the default logging level"        OFF )
OPTION( LOG_LEVEL "Set the default debug level; (-1 = OFF, 0 - Critical Message, 1 - Errors, 2 - Warnings, 3 - Status (default), 4 - Notes, 5 - Debug, 6 - All"   0 )  # OFF

IF( EXISTS "${POPSIM_INCLUDE_DIR}/popsim_config.h" )
    UNSET( ${POPSIM_INCLUDE_DIR}/popsim_config.h CACHE )
ENDIF()

CONFIGURE_FILE( ${POPSIM_INCLUDE_DIR}/popsim_config.h.in ${POPSIM_INCLUDE_DIR}/popsim_config.h)

set(CMAKE_CXX_FLAGS "-g -fPIC -Wall -O3" )

LINK_DIRECTORIES( ${PROJECT_BINARY_DIR}/bin )

add_subdirectory( src )
add_subdirectory( unittest )
