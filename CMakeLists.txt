#cmake_minimum_required(VERSION 2.6)
cmake_minimum_required( VERSION 2.4.3 FATAL_ERROR )



project ( pcietest ) 

#
# Setup the find commands
#
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeScripts)


set ( Pcietest_VERSION_MAJOR 1 )
set ( Pcietest_VERSION_MINOR 0 )
set ( COMMON_DIR "${PROJECT_SOURCE_DIR}/common" )



configure_file (
  "${PROJECT_SOURCE_DIR}/include/Pcietest.h.in"
  "${PROJECT_SOURCE_DIR}/include/Pcietest.h"
)

include_directories ( include  )

find_package(Doxygen REQUIRED)
find_package(LATEX REQUIRED )
find_package(Git REQUIRED )
find_package(Threads)

Option(BUILD_DOXYGEN "Build Doxygen" OFF)
  if(DOXYGEN_FOUND)
    MESSAGE(STATUS "*** Building the Doxygen documentaion ***")
    ADD_SUBDIRECTORY(doxygen)
endif(DOXYGEN_FOUND)

if (WIN32)
  add_library(doslib)
  add_subdirectory (dos)
  set(EXTRA_LIBS ${EXTRA_LIBS} MathFunctions)
  set(TEST_VARIABLE windows )
  set(CORELIBS )
endif(WIN32)

if (UNIX)
  find_package(Curses)
  include_directories ("/usr/include" 
                       "${PROJECT_SOURCE_DIR}/include"
                       "${PROJECT_SOURCE_DIR}/linux/include"
                       )
  set(TEST_VARIABLE linux )
  set(CORELIBS ${CMAKE_THREAD_LIBS_INIT} ${CURSES_LIBRARY} )
endif(UNIX)

add_executable( pcietest ${COMMON_DIR}/pcietest.cpp )
target_link_libraries( pcietest ${CORELIBS} )

