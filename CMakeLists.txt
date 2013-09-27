#cmake_minimum_required(VERSION 2.6)
cmake_minimum_required( VERSION 2.4.3 FATAL_ERROR )
if(COMMAND cmake_policy)
  # policy settings ...
   cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


project ( pcietest ) 

#
# Setup the find commands
#
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeScripts)


set ( Pcietest_VERSION_MAJOR 1 )
set ( Pcietest_VERSION_MINOR 0 )
set ( COMMON_DIR "${PROJECT_SOURCE_DIR}/common" )
set ( LIB_DIR "${PROJECT_SOURCE_DIR}/lib" )
set ( SAMPLES_DIR "${PROJECT_SOURCE_DIR}/samples" )



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
# if(DOXYGEN_FOUND)
#   MESSAGE(STATUS "*** Building the Doxygen documentaion ***")
# endif(DOXYGEN_FOUND)

option(BUILD_TESTLIB_SHARED "Build the testlib fixtures into a shared library" ON )
option(BUILD_SHARED_LIBS "Build all shared libraries" ON )


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

#
# Build a library for the irq wrapping
#

add_library(test_utilities "${PROJECT_SOURCE_DIR}/common/display.c"
                           "${PROJECT_SOURCE_DIR}/common/common_objects.cpp"
                           "${PROJECT_SOURCE_DIR}/common/iiro16.cpp"
                           "${PROJECT_SOURCE_DIR}/common/iiro8.cpp" 
                           "${PROJECT_SOURCE_DIR}/common/8254ctr.cpp"
                           "${PROJECT_SOURCE_DIR}/linux/io.cpp" 
                           "${PROJECT_SOURCE_DIR}/linux/irq.cpp" 
                           "${PROJECT_SOURCE_DIR}/linux/pciutil.cpp" 
                           )
                           
link_directories (${LIB_DIR}/test_utilities)


  set(CORELIBS ${CMAKE_THREAD_LIBS_INIT} ${CURSES_LIBRARY} test_utilities)
endif(UNIX)

add_executable( pci_da12_waveforms ${SAMPLES_DIR}/pci_da12_waveforms.cpp )
target_link_libraries( pci_da12_waveforms ${CORELIBS} curses)

add_executable( pci_da12_setvoltages ${SAMPLES_DIR}/pci_da12_setvoltages.cpp )
target_link_libraries( pci_da12_setvoltages ${CORELIBS} curses)

add_executable( pci_da12_basic ${SAMPLES_DIR}/pci_da12_basic.cpp )
target_link_libraries( pci_da12_basic ${CORELIBS} curses)

add_executable( pci_da12 ${SAMPLES_DIR}/pci_da12.cpp )
target_link_libraries( pci_da12 ${CORELIBS} curses)


add_executable( pcietest ${COMMON_DIR}/pcietest.cpp )
target_link_libraries( pcietest ${CORELIBS} )


add_executable( iiro16_test ${SAMPLES_DIR}/iiro16_test.cpp )
target_link_libraries( iiro16_test ${CORELIBS} )

add_executable( iiro8_test ${SAMPLES_DIR}/iiro8_test.cpp )
target_link_libraries( iiro8_test ${CORELIBS} )


