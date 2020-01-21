cmake_minimum_required (VERSION 3.8)

FILE(GLOB_RECURSE Meshes *.obj)

set(ENGINE_DIR ../Engine/Assets/)

foreach(file ${MESHES})
    FILE(RELATIVE_PATH ITEM_PATH_REL . ${MESHES})
    configure_file(${ITEM_PATH_REL} ${CMAKE_CURRENT_BINARY_DIR}/${ENGINE_DIR}${ITEM_PATH_REL} COPYONLY)
endforeach()

FILE(GLOB Programs Programs/*.shader)

foreach(file ${PROGRAMS})
    FILE(RELATIVE_PATH ITEM_PATH_REL . ${PROGRAMS})
    configure_file(${ITEM_PATH_REL} ${CMAKE_CURRENT_BINARY_DIR}/${ENGINE_DIR}${ITEM_PATH_REL} COPYONLY)
endforeach()

FILE(GLOB Shaders Shaders/*.vert Shaders/*.frag)

foreach(SHADER ${SHADERS})
    FILE(RELATIVE_PATH ITEM_PATH_REL . ${SHADER})
    configure_file(${ITEM_PATH_REL} ${CMAKE_CURRENT_BINARY_DIR}/${ENGINE_DIR}${ITEM_PATH_REL} COPYONLY)
endforeach()