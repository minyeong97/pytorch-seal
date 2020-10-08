# Install script for directory: /home/vimsiin/Desktop/seal/native/src/seal/util

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SEAL-3.5/seal/util" TYPE FILE FILES
    "/home/vimsiin/Desktop/seal/native/src/seal/util/blake2.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/blake2-impl.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/clang.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/clipnormal.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/common.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/config.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/croots.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/defines.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/galois.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/gcc.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/globals.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/hash.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/hestdparms.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/iterator.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/locks.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/mempool.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/msvc.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/numth.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/pointer.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/polyarithsmallmod.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/polycore.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/rlwe.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/rns.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/scalingvariant.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/ntt.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/streambuf.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/uintarith.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/uintarithmod.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/uintarithsmallmod.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/uintcore.h"
    "/home/vimsiin/Desktop/seal/native/src/seal/util/ztools.h"
    )
endif()

