# Install script for directory: /home/wuxx1279/KR/simple

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
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
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
  set(CMAKE_INSTALL_SO_NO_EXE "0")
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
  if(EXISTS "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetInserter" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetInserter")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetInserter"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/wuxx1279/KR/simple/build/bin/snippetInserter")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/wuxx1279/KR/simple/build/bin" TYPE EXECUTABLE FILES "/home/wuxx1279/KR/simple/build/snippetInserter/snippetInserter")
  if(EXISTS "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetInserter" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetInserter")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetInserter"
         OLD_RPATH "/home/wuxx1279/spack/opt/spack/linux-centos8-zen/gcc-8.4.1/boost-1.76.0-yznhznjyr57i3k7kh4rhokufjwcynr7h/lib:/home/wuxx1279/bin/dyninst/lib:/home/wuxx1279/spack/opt/spack/linux-centos8-zen/gcc-8.4.1/intel-tbb-2020.3-zsxf733fiiixymsiegwii7abxzhidclv/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetInserter")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetGenerator" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetGenerator")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetGenerator"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/wuxx1279/KR/simple/build/bin/snippetGenerator")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/wuxx1279/KR/simple/build/bin" TYPE EXECUTABLE FILES "/home/wuxx1279/KR/simple/build/snippetGenerator/snippetGenerator")
  if(EXISTS "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetGenerator" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetGenerator")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetGenerator"
         OLD_RPATH "/home/wuxx1279/spack/opt/spack/linux-centos8-zen/gcc-8.4.1/boost-1.76.0-yznhznjyr57i3k7kh4rhokufjwcynr7h/lib:/home/wuxx1279/bin/dyninst/lib:/home/wuxx1279/spack/opt/spack/linux-centos8-zen/gcc-8.4.1/intel-tbb-2020.3-zsxf733fiiixymsiegwii7abxzhidclv/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetGenerator")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetReader" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetReader")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetReader"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/wuxx1279/KR/simple/build/bin/snippetReader")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/wuxx1279/KR/simple/build/bin" TYPE EXECUTABLE FILES "/home/wuxx1279/KR/simple/build/snippetReader/snippetReader")
  if(EXISTS "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetReader" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetReader")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetReader"
         OLD_RPATH "/home/wuxx1279/spack/opt/spack/linux-centos8-zen/gcc-8.4.1/boost-1.76.0-yznhznjyr57i3k7kh4rhokufjwcynr7h/lib:/home/wuxx1279/bin/dyninst/lib:/home/wuxx1279/spack/opt/spack/linux-centos8-zen/gcc-8.4.1/intel-tbb-2020.3-zsxf733fiiixymsiegwii7abxzhidclv/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/wuxx1279/KR/simple/build/bin/snippetReader")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/wuxx1279/KR/simple/build/snippetInserter/cmake_install.cmake")
  include("/home/wuxx1279/KR/simple/build/snippetGenerator/cmake_install.cmake")
  include("/home/wuxx1279/KR/simple/build/snippetReader/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/wuxx1279/KR/simple/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
