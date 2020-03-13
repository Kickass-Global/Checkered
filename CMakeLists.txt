# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.12)

set(MSVC_COMPILE_FLAGS "/MP")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_COMPILE_FLAGS}")

project("Engine")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

set(FREETYPE_DIR 3rd-party/freetype-2.10.0)

add_subdirectory(3rd-party/freetype-2.10.0)

add_library(SOIL 3rd-party/SOIL/srcs/etc1_utils.c
        3rd-party/SOIL/srcs/image_DXT.c
        3rd-party/SOIL/srcs/image_helper.c
        3rd-party/SOIL/srcs/SOIL2.c)

target_include_directories(SOIL PUBLIC 3rd-party/SOIL/incs)

target_link_libraries(SOIL
        PRIVATE ${OPENGL_gl_LIBRARY}
        )


# Include sub-projects.
add_subdirectory("Engine")






