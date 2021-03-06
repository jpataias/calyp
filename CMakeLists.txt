###
### CMakeLists for calyp
###

cmake_minimum_required( VERSION 3.2.0 )

######################################################################################
# CMake  functions
######################################################################################
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
INCLUDE( cmake/ModulesFct.cmake )
INCLUDE(FeatureSummary)
INCLUDE(GNUInstallDirs)

######################################################################################
# Project Definition
######################################################################################

SET( PROJECT_NAME "calyp" CACHE INTERNAL "" FORCE )
SET( PROJECT_LIBRARY "Calyp" CACHE INTERNAL "" FORCE )


PROJECT( ${PROJECT_NAME} )
SET( CALYP_VERSION_MAJOR "1" )
SET( CALYP_VERSION_MINOR "1" )
SET( CALYP_VERSION_PATCH "0" )
SET( CALYP_VERSION "${CALYP_VERSION_MAJOR}.${CALYP_VERSION_MINOR}.${CALYP_VERSION_PATCH}" CACHE INTERNAL "" FORCE )

######################################################################################
# Variables
######################################################################################

OPTION( BUILD_APP               "Build GUI Application"                 ON  )
OPTION( BUILD_TOOLS             "Build Command Line Application"        ON  )
OPTION( BUILD_EXAMPLES          "Build Examples"                        OFF )
OPTION( BUILD_DOC               "Build Documentation"                   OFF )
OPTION( BUILD_TESTS             "Build Google Tests"                    OFF )

OPTION( USE_FERVOR              "Add Fervor support"                    OFF )

OPTION( USE_DYNLOAD             "Use dynamic load of modules"           ON  )
OPTION( USE_SSE                 "Build with SSE support"                OFF )
OPTION( USE_WERROR              "Warnings as errors"                    OFF )
OPTION( USE_STATIC              "Use static libs"                       OFF )


######################################################################################
# Release configuration
######################################################################################

# To be switched on when releasing.
OPTION(RELEASE_BUILD "Remove Git revision from program version (use for stable releases)" ON)

# Get current version.
set(CALYP_VERSION_STRING "${CALYP_VERSION}")
if(NOT RELEASE_BUILD AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
  # Probably a Git workspace; determine the revision.
  FIND_PACKAGE(Git QUIET)
  if(GIT_FOUND)
    exec_program(${GIT_EXECUTABLE}
      ${CMAKE_CURRENT_SOURCE_DIR}
      ARGS "describe --tags"
      OUTPUT_VARIABLE CALYP_GIT_REVISION
      RETURN_VALUE TAG_RESULT
    )
    # git describe --tags failed; maybe the repository was checked with depth=1.
    if(NOT ${TAG_RESULT} EQUAL 0)
      exec_program(${GIT_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ARGS "describe --always"
        OUTPUT_VARIABLE CALYP_GIT_REVISION
      )
    endif()
    message(STATUS "Calyp Git revision: ${CALYP_GIT_REVISION}")
    set(CALYP_VERSION_STRING "${CALYP_VERSION} (rev. ${CALYP_GIT_REVISION})")
  else()
    message(STATUS "Calyp Git revision could not be determined")
  endif()
endif()

SET(UPDATE_CHANNEL "stable" CACHE STRING "Update channel" )
# CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/CalypUpdate.xml.in ${CMAKE_CURRENT_BINARY_DIR}/CalypUpdate-${UPDATE_CHANNEL}.xml )


######################################################################################
# Dependencies
######################################################################################

IF( BUILD_DOC )
  FIND_PACKAGE(Doxygen)
  SET( BUILD_DOC ${Doxygen_FOUND} )
ENDIF()
SET_PACKAGE_PROPERTIES(Doxygen PROPERTIES URL "http://www.doxygen.org/" DESCRIPTION "Doxygen" TYPE OPTIONAL)


IF( BUILD_TESTS )
  FIND_PACKAGE(GTest)
  SET( BUILD_DOC ${GTEST_FOUND} )
  IF( GTEST_FOUND )
    ENABLE_TESTING()
    INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})
  ENDIF()
ENDIF()

OPTION( USE_OPENCV "Add OpenCV support" ON )
IF( USE_OPENCV )

  FIND_PACKAGE( OpenCV COMPONENTS core highgui imgproc video )
  IF( ${OpenCV_FOUND} )
    SET(USE_OPENCV TRUE )
    LIST(APPEND OPENCV_MODULES core highgui imgproc video )
    FIND_PACKAGE( OpenCV COMPONENTS videoio photo )
    IF( ${OPENCV_VIDEOIO_FOUND} )
      LIST(APPEND OPENCV_MODULES videoio )
    ENDIF()
    IF( ${OPENCV_PHOTO_FOUND} )
      LIST(APPEND OPENCV_MODULES photo )
    ENDIF()
  ELSE()
    SET(USE_OPENCV FALSE )
  ENDIF()
ENDIF()
SET_PACKAGE_PROPERTIES(OpenCV PROPERTIES URL "http://opencv.willowgarage.com" DESCRIPTION "OpenCV library support in CalypFrame" TYPE OPTIONAL)

OPTION( USE_FFMPEG "Add FFMPEG support" ON )
IF( USE_FFMPEG )
  FIND_PACKAGE( FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE )
  IF( "${AVCODEC_VERSION}" VERSION_LESS "56.0.0" )
    SET(FFMPEG_FOUND FALSE )
    SET(FFmpeg_FOUND FALSE )
  ENDIF()
  SET(USE_FFMPEG ${FFMPEG_FOUND})
ENDIF()
SET_PACKAGE_PROPERTIES(FFmpeg PROPERTIES URL "http://ffmpeg.org/" DESCRIPTION "Libav library support in CalypStream" TYPE OPTIONAL)

IF( WIN32 )
  SET( USE_STATIC ON )
  INCLUDE( cmake/Win32.cmake )
ENDIF()


######################################################################################
# CMake Defin1itions
######################################################################################

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
INCLUDE(FeatureSummary)

# must go before the project command
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
IF(DEFINED CMAKE_BUILD_TYPE AND CMAKE_VERSION VERSION_GREATER "2.8")
  SET_PROPERTY( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES} )
ENDIF()
SET( CMAKE_CXX_FLAGS "" )

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

######################################################################################
# Compiler options
######################################################################################

SET(CMAKE_CXX_STANDARD 14)
if(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=gnu++0x)
endif()

IF (NOT CMAKE_BUILD_TYPE )
    MESSAGE(STATUS "No build type selected, default to Release")
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )

ADD_DEFINITIONS(-DQT_NO_CAST_TO_ASCII
                #-DQT_NO_CAST_FROM_ASCII
                -DQT_NO_URL_CAST_FROM_STRING
                -DQT_NO_CAST_FROM_BYTEARRAY
                #-DQT_NO_SIGNALS_SLOTS_KEYWORDS
                #-DQT_USE_QSTRINGBUILDER
               )

# if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
# endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
   if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0.0")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wlogical-op" )
   endif()
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations" )
endif()

IF( USE_WERROR )
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror" )
ENDIF()

SET( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0" )
SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2" )


######################################################################################
# Configure targets
######################################################################################

ADD_FEATURE_INFO(CalypApp     BUILD_APP    "Build Graphical interface")
ADD_FEATURE_INFO(CalypTools   BUILD_TOOLS  "Build Command line tool"  )
ADD_FEATURE_INFO(DynLoad    USE_DYNLOAD   "Support for dynamic module load" )
ADD_FEATURE_INFO(SSE        USE_SSE       "SSE instructions support" )
ADD_FEATURE_INFO(WErrors    USE_WERROR    "Warnings as errors" )

ADD_SUBDIRECTORY( lib )
ADD_SUBDIRECTORY( modules )

IF( ${BUILD_TOOLS} )
  ADD_SUBDIRECTORY( tools )
ENDIF()

IF( ${BUILD_APP} )
  ADD_SUBDIRECTORY( app )
ENDIF()

IF( ${BUILD_EXAMPLES} )
  ADD_SUBDIRECTORY( examples )
ENDIF()


CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h )

######################################################################################
# Final information dump
######################################################################################
MESSAGE("")
MESSAGE( STATUS "${PROJECT_NAME} INFORMATION"                     )
MESSAGE( STATUS "Version: "                 "${CALYP_VERSION_STRING}" )
MESSAGE( STATUS "Configuration:"                                  )
MESSAGE( STATUS "    Static libs: "         "${USE_STATIC}" )
MESSAGE( STATUS "    Build type: "          "${CMAKE_BUILD_TYPE}" )
MESSAGE( STATUS "    Build flags: "         "${CMAKE_CXX_FLAGS}"  )

FEATURE_SUMMARY(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
