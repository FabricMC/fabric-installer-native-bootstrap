cmake_minimum_required(VERSION 3.17)

if(APPLE)
    # Build universal x86_64/arm64 MacOS binary.
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum X64 deployment version" FORCE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Architectures" FORCE)
    set(CMAKE_XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET[arch=arm64] "11.0" CACHE STRING "Minimum ARM64 deployment version" FORCE)
endif()

project(fabric-installer-native-bootstrap)
set(CMAKE_CXX_STANDARD 20)

set(PLATFORM_IMPL src/nix/PlatformLinux.cpp)
IF(APPLE)
    set(PLATFORM_IMPL src/nix/PlatformMacOS.mm)
endif()

add_executable(fabric-installer-native-bootstrap src/nix/main.cpp ${PLATFORM_IMPL})

if(UNIX AND NOT APPLE)
    # Static link
    target_link_libraries(fabric-installer-native-bootstrap -static)
endif()

if(APPLE)
    target_link_libraries(fabric-installer-native-bootstrap "-framework Cocoa")
endif()

# Warnings as errors.
if(MSVC)
    target_compile_options(fabric-installer-native-bootstrap PRIVATE /W4 /WX)
elseif(NOT APPLE) # TODO: Fix MacOS warnings
    target_compile_options(fabric-installer-native-bootstrap PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()