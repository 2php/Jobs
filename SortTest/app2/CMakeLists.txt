project(App2)
cmake_minimum_required(VERSION 2.8)

#source add
aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

include_directories(${PROJECT_SOURCE_DIR})


#
IF (WIN32)
    #add pthread
    include_directories(${PROJECT_SOURCE_DIR}/../pthread-win32/include)
    link_directories(${PROJECT_SOURCE_DIR}/../pthread-win32/lib/x86)

    #define
    ADD_DEFINITIONS(-DWIN32)

    MESSAGE(STATUS "Now is windows")
    add_library(
        pthread
        SHARED
        IMPORTED)
    set_target_properties(
        pthread
        PROPERTIES
        IMPORTED_IMPLIB
        ${PROJECT_SOURCE_DIR}/../pthread-win32/lib/x86/pthreadVC2.lib)
ELSEIF (APPLE)
        MESSAGE(STATUS "Now is Apple systens.")
ELSEIF (UNIX)
        MESSAGE(STATUS "Now is UNIX-like OS's. Including aPPLE os x  and CygWin")
ENDIF ()

#link config
target_link_libraries(
    ${PROJECT_NAME}
    pthread
    )

