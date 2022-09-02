cmake_minimum_required(VERSION 3.17)

if(APPLE)
    # Build universal x86_64/arm64 MacOS binary.
    set (CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum X64 deployment version" FORCE)
    set (CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Architectures" FORCE)
    set (CMAKE_XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET[arch=arm64] "11.0" CACHE STRING "Minimum ARM64 deployment version" FORCE)
endif()

project(fabric-installer-native-bootstrap)
set(CMAKE_CXX_STANDARD 20)

add_executable(fabric-installer-native-bootstrap src/nix/main.cpp)