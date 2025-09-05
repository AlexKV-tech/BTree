# set minimum cmake version
cmake_minimum_required(VERSION 3.15)
project(btree_impl)

# set compiler options
set(CMAKE_CXX_STANDARD 20)        # Use C++20 standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)     # Enforce standard compliance

# Export compile commands for LSP
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable debugging symbols
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")

# add source files
set(SOURCES
    src/main.cpp
)

# create the executable
add_executable(btree_impl ${SOURCES})

# include directories
target_include_directories(btree_impl PRIVATE ${CMAKE_SOURCE_DIR}/include)



# optional: enable warnings and debugging flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(btree_impl PRIVATE -Wall -Wextra -Wpedantic -g)
endif()