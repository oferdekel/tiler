cmake_minimum_required(VERSION 3.5)

project(tiler)
set(target_name tiler)

# files
set(include
    include/ForAll.h
    include/IndentedOutputStream.h
    include/Matrix.h
)

set(src
    src/ForAll.cpp
    src/Main.cpp
    src/Matrix.cpp
    src/IndentedOutputStream.cpp
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set C++ version
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# create executable in build\bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
add_executable(${target_name} ${src} ${include})
target_include_directories(${target_name} PRIVATE include)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")

