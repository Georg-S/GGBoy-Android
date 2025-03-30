# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")

project("GGBoy-Android")

set(HEADERS
        "include/Audio.hpp"
        "include/Video.hpp"
        "include/InputHandler.hpp"
        "include/EmulatorMain.hpp"
        )

set(SOURCES
        "src/Audio.cpp"
        "src/Video.cpp"
        "src/InputHandler.cpp"
        "src/EmulatorMain.cpp"
        "src/EmulatorAPI.cpp"
	    )

add_subdirectory(GGBoy-Core)

# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be
# used in the AndroidManifest.xml file.
add_library(${CMAKE_PROJECT_NAME} SHARED
        ${HEADERS}
        ${SOURCES})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "include")

# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        log
        GGBoyCore
        AAudio)