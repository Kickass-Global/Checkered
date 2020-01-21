cmake_minimum_required(VERSION 3.12)

message(STATUS "Copying Assets")

FILE(GLOB_RECURSE MESHES ${CMAKE_CURRENT_SOURCE_DIR}/Meshes *.obj)
foreach(file ${MESHES})
    message(STATUS "Copying " ${file})
    FILE(RELATIVE_PATH ITEM_PATH_REL ${CMAKE_CURRENT_SOURCE_DIR} ${MESHES})
    configure_file(${ITEM_PATH_REL} ${CMAKE_CURRENT_BINARY_DIR}/${ITEM_PATH_REL} COPYONLY)
endforeach()

FILE(GLOB_RECURSE PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/Programs *.shader)
foreach(file ${PROGRAMS})
    message(STATUS "Copying " ${file})
    FILE(RELATIVE_PATH ITEM_PATH_REL ${CMAKE_CURRENT_SOURCE_DIR} ${PROGRAMS})
    configure_file(${ITEM_PATH_REL} ${CMAKE_CURRENT_BINARY_DIR}/${ITEM_PATH_REL} COPYONLY)
endforeach()

FILE(GLOB_RECURSE SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/Shaders *.vert *.frag)
foreach (file ${SHADERS})
    message(STATUS "Copying " ${file})
    FILE(RELATIVE_PATH ITEM_PATH_REL ${CMAKE_CURRENT_SOURCE_DIR} ${file})
    configure_file(${ITEM_PATH_REL} ${CMAKE_CURRENT_BINARY_DIR}/${ITEM_PATH_REL} COPYONLY)
endforeach()