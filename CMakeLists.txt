################################################################################
# Copyright (c) 2013, Patrick P. Putnam (putnampp@gmail.com)
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met: 
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer. 
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution. 
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are those
# of the authors and should not be interpreted as representing official policies,
# either expressed or implied, of the FreeBSD Project.
################################################################################
cmake_minimum_required (VERSION 2.8)
PROJECT( CLOTHO CXX )

SET (${PROJECT_NAME}_LIB_CORENAME "clotho")

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

SET (${PROJECT_NAME}_VER_MAJ 0)
SET (${PROJECT_NAME}_VER_MIN 1)
SET (${PROJECT_NAME}_VER_RELEASE 0 )

SET (${PROJECT_NAME}_RESOURCES_DIR ${${PROJECT_NAME}_SOURCE_DIR}/cmake)
SET (${PROJECT_NAME}_SRC_DIR       ${${PROJECT_NAME}_SOURCE_DIR}/src)
SET( ${PROJECT_NAME}_INCLUDE_DIR   ${${PROJECT_NAME}_SOURCE_DIR}/include )
SET( ${PROJECT_NAME}_UNITTEST_DIR   ${${PROJECT_NAME}_SOURCE_DIR}/unittest )

SET (CMAKE_INSTALL_PREFIX "./popsim" CACHE PATH "Install path prefix, prepended onto install directories")

IF (NOT ${PROJECT_NAME}_INSTALL_BIN_DIR )
    SET (${PROJECT_NAME}_INSTALL_BIN_DIR bin )
ENDIF (NOT ${PROJECT_NAME}_INSTALL_BIN_DIR )

IF (NOT ${PROJECT_NAME}_INSTALL_LIB_DIR )
    SET (${PROJECT_NAME}_INSTALL_LIB_DIR lib )
ENDIF (NOT ${PROJECT_NAME}_INSTALL_LIB_DIR )

IF (NOT ${PROJECT_NAME}_INSTALL_INCLUDE_DIR )
    SET(${PROJECT_NAME}_INSTALL_INCLUDE_DIR include)
ENDIF (NOT ${PROJECT_NAME}_INSTALL_INCLUDE_DIR )

################################################################################
SET ( ${PROJECT_NAME}_PACKAGE "popsim" )
SET ( ${PROJECT_NAME}_PACKAGE_NAME "${PROJECT_NAME}" )
SET (${PROJECT_NAME}_PACKAGE_VERSION "${${PROJECT_NAME}_VER_MAJ}.${${PROJECT_NAME}_VER_MIN}.${${PROJECT_NAME}_VER_RELEASE}")
SET (${PROJECT_NAME}_PACKAGE_VER_MAJOR "${${PROJECT_NAME}_VER_MAJ}.${${PROJECT_NAME}_VER_MIN}")
SET (${PROJECT_NAME}_PACKAGE_VER_MINOR "${${PROJECT_NAME}_VER_RELEASE}")
SET (${PROJECT_NAME}_PACKAGE_STRING "${${PROJECT_NAME}_PACKAGE_NAME} ${${PROJECT_NAME}_PACKAGE_VERSION}")
SET (${PROJECT_NAME}_PACKAGE_BUGREPORT "putnampp@gmail.com")

MESSAGE( STATUS ${${PROJECT_NAME}_PACKAGE_STRING} )

IF (NOT ${PROJECT_NAME}_EXTERNALLY_CONFIGURED )
    SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY 
        ${PROJECT_BINARY_DIR}/bin CACHE PATH "Executable directory")
    SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/lib CACHE PATH "Libraries directory")
    SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/lib CACHE PATH "Static Libraries directory")
ELSE (NOT ${PROJECT_NAME}_EXTERNALLY_CONFIGURED )
    IF (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY )
        SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH} )
    ENDIF (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY )
ENDIF (NOT ${PROJECT_NAME}_EXTERNALLY_CONFIGURED )

################################################################################
FIND_PACKAGE( Boost COMPONENTS program_options filesystem system thread unit_test_framework chrono REQUIRED )
IF( Boost_FOUND )
    INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
    MESSAGE (STATUS "Boost Include: " ${Boost_INCLUDE_DIR} )
    MESSAGE (STATUS "Boost Library: " ${Boost_LIBRARIES} )
ENDIF( Boost_FOUND )
LIST( APPEND COMMON_LIBRARIES ${Boost_LIBRARIES} )

#FIND_PACKAGE( CUDA REQUIRED )
#IF( CUDA_FOUND )
#    MESSAGE(STATUS "CUDA Include: " ${CUDA_INCLUDE_DIRS} )
#    MESSAGE(STATUS "CUDA Library: " ${CUDA_LIBRARIES} )
#ENDIF( CUDA_FOUND )
#LIST( APPEND COMMON_LIBRARIES ${CUDA_LIBRARIES} )

FIND_PACKAGE( GSL REQUIRED )
IF( GSL_FOUND )
    MESSAGE( STATUS "GSL Library: " ${GSL_LIBRARIES} )
ENDIF( GSL_FOUND )
LIST( APPEND COMMON_LIBRARIES ${GSL_LIBRARIES} )

SET( Warped_DIR "/opt/warped" CACHE PATH "Path to warped libraries")
#SET( Warped_LIBRARIES "/opt/warped/utils/lib/libutils.so/opt/warped/warped/lib/libwarped.so")
MESSAGE( STATUS "Warped DIR: " ${Warped_DIR})

SET( Warped_INCLUDE_DIRS "${Warped_DIR}/include")
SET( Warped_LIBRARIES "${Warped_DIR}/lib")

SET( Gperftools_DIR "/opt/gperftools/latest" )
find_library( TCMALLOC_LIBRARY NAMES tcmalloc PATHS ${Gperftools_DIR}/lib )
find_library( PROFILER_LIBRARY NAMES profiler PATHS ${Gperftools_DIR}/lib )

MESSAGE( STATUS "TCMALLOC: " ${TCMALLOC_LIBRARY})
LIST( APPEND COMMON_LIBRARIES ${TCMALLOC_LIBRARY} )
MESSAGE( STATUS "PROFILER: " ${PROFILER_LIBRARY})
LIST( APPEND COMMON_LIBRARIES ${PROFILER_LIBRARY} )

#SET( YAML_LIB "yaml-cpp" )
#LIST( APPEND COMMON_LIBRARIES ${YAML_LIB} )

find_library( REALTIME_LIB NAMES rt )
LIST( APPEND COMMON_LIBRARIES ${REALTIME_LIB} )

find_library( PTHREAD_LIB NAMES pthread )
LIST( APPEND COMMON_LIBRARIES ${PTHREAD_LIB})

FOREACH( L ${COMMON_LIBRARIES} ) 
    MESSAGE( STATUS ${L} )
ENDFOREACH(L)


################################################################################

OPTION( LOG       "Compile with debugging; sets LOG_LEVEL to the default logging level"        OFF )
OPTION( LOG_LEVEL "Set the default debug level; (-1 = OFF, 0 - Critical Message, 1 - Errors, 2 - Warnings, 3 - Status (default), 4 - Notes, 5 - Debug, 6 - All"   0 )  # OFF

SET(MAX_ALLELES 256 CACHE INTEGER "Maximum alleles per locus")
SET(PWORD_SIZE 64 CACHE INTEGER "Bits per processor word")

IF( MAX_ALLELES LESS 0 )
    MESSAGE(FATAL_ERROR "Maximum alleles cannot be negative" )
ENDIF()

IF(PWORD_SIZE LESS 0 )
    MESSAGE(FATAL_ERROR "Processor word size cannot be negative")
ENDIF()

IF( EXISTS "${${PROJECT_NAME}_INCLUDE_DIR}/config.h" )
    UNSET( ${${PROJECT_NAME}_INCLUDE_DIR}/config.h CACHE )
ENDIF()

CONFIGURE_FILE( ${${PROJECT_NAME}_INCLUDE_DIR}/config.h.in ${${PROJECT_NAME}_INCLUDE_DIR}/config.h)
CONFIGURE_FILE( ${${PROJECT_NAME}_INCLUDE_DIR}/cchmc/config.hpp.in ${${PROJECT_NAME}_INCLUDE_DIR}/cchmc/config.hpp)

CONFIGURE_FILE( ${${PROJECT_NAME}_UNITTEST_DIR}/test_main.cpp.in ${${PROJECT_NAME}_UNITTEST_DIR}/test_main.cpp )

set(CMAKE_CXX_FLAGS "-g -fPIC -Wall -O3 -std=c++0x -march=native -pedantic" )

LINK_DIRECTORIES( ${PROJECT_BINARY_DIR}/bin )

add_subdirectory( src )
add_subdirectory( unittest )
