# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.12)

project("Engine")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(SOIL 3rd-party/SOIL/srcs/etc1_utils.c
        3rd-party/SOIL/srcs/image_DXT.c
        3rd-party/SOIL/srcs/image_helper.c
        3rd-party/SOIL/srcs/SOIL2.c)

target_include_directories(SOIL PUBLIC 3rd-party/SOIL/incs)


# Include sub-projects.
add_subdirectory("Engine")





