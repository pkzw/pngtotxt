# CMakeLists.txt for pngtotxt

cmake_minimum_required (VERSION 3.5)

project(pngtotxt LANGUAGES C)

# Version number of pngtotxt
set(pngtotxt_VERSION_MAJOR 0)
set(pngtotxt_VERSION_MINOR 9)
set(pngtotxt_VERSION_PATCH 0)

option (ENABLE_BINRELOC "compile with binary relocation support" ON)

if(WIN32)
  message(STATUS "Building for Windows")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
else(WIN32)
  message(STATUS "Building for non-Windows platform")  
endif(WIN32)

find_package(Iconv REQUIRED)
find_package(PNG 1.6 REQUIRED)

add_subdirectory(include)
add_subdirectory(src)

# Uninstall target
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
