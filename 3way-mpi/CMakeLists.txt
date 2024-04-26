cmake_minimum_required(VERSION 3.14)

# ALL OF THE SPECIFIED NAMES IN THIS CMAKE ARE REQUIRED.
# Set the project name variable to assignment3.
# THIS IS REQUIRED
project(pt2)


# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add our include directory to CMake's search paths.
# THIS IS REQUIRED
include_directories(include)

# Compile the analysis executable.
add_executable(pt2 pt2.c)


# Link ${PROJECT_NAME}_test with dyn_array and gtest and pthread libraries
#target_link_libraries(${PROJECT_NAME} pthread)

find_package(MPI)

include_directories(SYSTEM ${MPI_INCLUDE_PATH})

target_link_libraries(${PROJECT_NAME} ${MPI_C_LIBRARIES})
