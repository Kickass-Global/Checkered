cmake_minimum_required(VERSION 3.12)

message(STATUS "Copying Assets")

FILE(GLOB_RECURSE MESHES ${CMAKE_CURRENT_SOURCE_DIR}/Meshes *.obj *.fbx)
foreach(file ${MESHES})
    FILE(RELATIVE_PATH ITEM_PATH_REL ${CMAKE_CURRENT_SOURCE_DIR} ${file})
    message(STATUS "Copying "  ${CMAKE_CURRENT_BINARY_DIR}/../Engine/Assets/${ITEM_PATH_REL})
    configure_file(${ITEM_PATH_REL} ${CMAKE_CURRENT_BINARY_DIR}/../Engine/Assets/${ITEM_PATH_REL} COPYONLY)
endforeach()

FILE(GLOB_RECURSE PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR} *.json)
foreach(file ${PROGRAMS})
    FILE(RELATIVE_PATH ITEM_PATH_REL ${CMAKE_CURRENT_SOURCE_DIR} ${file})
    message(STATUS "Copying "  ${CMAKE_CURRENT_BINARY_DIR}/../Engine/Assets/${ITEM_PATH_REL})
    configure_file(${ITEM_PATH_REL} ${CMAKE_CURRENT_BINARY_DIR}/../Engine/Assets/${ITEM_PATH_REL} COPYONLY)
endforeach()

FILE(GLOB_RECURSE SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/Shaders *.vert *.frag)
foreach (file ${SHADERS})
    FILE(RELATIVE_PATH ITEM_PATH_REL ${CMAKE_CURRENT_SOURCE_DIR} ${file})
    message(STATUS "Copying "  ${CMAKE_CURRENT_BINARY_DIR}/../Engine/Assets/${ITEM_PATH_REL})
    configure_file(${ITEM_PATH_REL} ${CMAKE_CURRENT_BINARY_DIR}/../Engine/Assets/${ITEM_PATH_REL} COPYONLY)
endforeach ()

FILE(GLOB_RECURSE SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/Sounds *.wav)
foreach (file ${SHADERS})
    FILE(RELATIVE_PATH ITEM_PATH_REL ${CMAKE_CURRENT_SOURCE_DIR} ${file})
    message(STATUS "Copying "  ${CMAKE_CURRENT_BINARY_DIR}/../Engine/Assets/${ITEM_PATH_REL})
    configure_file(${ITEM_PATH_REL} ${CMAKE_CURRENT_BINARY_DIR}/../Engine/Assets/${ITEM_PATH_REL} COPYONLY)
endforeach ()


FILE(GLOB_RECURSE SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/Textures *.png *.jpg *.jpeg)
foreach (file ${SHADERS})
    FILE(RELATIVE_PATH ITEM_PATH_REL ${CMAKE_CURRENT_SOURCE_DIR} ${file})
    message(STATUS "Copying "  ${CMAKE_CURRENT_BINARY_DIR}/../Engine/Assets/${ITEM_PATH_REL})
    configure_file(${ITEM_PATH_REL} ${CMAKE_CURRENT_BINARY_DIR}/../Engine/Assets/${ITEM_PATH_REL} COPYONLY)
endforeach ()