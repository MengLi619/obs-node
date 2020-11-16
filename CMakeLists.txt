cmake_minimum_required(VERSION 3.13)
project(obs-node)

set(CMAKE_CXX_STANDARD 17)

if (NOT OBS_STUDIO_DIR)
    set(OBS_STUDIO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/obs-studio-build/obs-installed)
endif()

message(STATUS "'OBS_STUDIO_DIR' = ${OBS_STUDIO_DIR}")

# Include QT for linux
if(UNIX AND NOT APPLE)
    find_package(Qt5X11Extras REQUIRED)
    find_package(Qt5Widgets ${FIND_MODE})
    if(NOT Qt5Widgets_FOUND)
        message(FATAL_ERROR "Failed to find Qt5")
    endif()
endif()

# Build
SET(OBS_NODE_SOURCES
    src/cpp/main.cpp
    src/cpp/utils.h
    src/cpp/settings.h
    src/cpp/settings.cpp
    src/cpp/studio.h
    src/cpp/studio.cpp
    src/cpp/source.h
    src/cpp/source.cpp
    src/cpp/scene.h
    src/cpp/scene.cpp
    src/cpp/display.h
    src/cpp/display.cpp
    src/cpp/dsk.h
    src/cpp/dsk.cpp
    src/cpp/platform/platform.h)

if (WIN32)
    LIST(APPEND OBS_NODE_SOURCES
        src/cpp/platform/windows.cpp
    )
elseif(APPLE)
    LIST(APPEND OBS_NODE_SOURCES
            src/cpp/platform/osx.mm
    )
endif()

add_library(${PROJECT_NAME} SHARED ${OBS_NODE_SOURCES} ${CMAKE_JS_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# Include
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
)
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_JS_INC}
        ${NODE_ADDON_API_DIR}
        ${OBS_STUDIO_DIR}/include
)

# Linking
if (APPLE)
    LIST(APPEND OBS_NODE_DEPS
        ${OBS_STUDIO_DIR}/bin/libobs.dylib
    )
elseif(UNIX)
    LIST(APPEND OBS_NODE_DEPS
        ${OBS_STUDIO_DIR}/bin/64bit/libobs.so
        Qt5::Widgets
    )
elseif(WIN32)
    LIST(APPEND OBS_NODE_DEPS
        ${OBS_STUDIO_DIR}/bin/64bit/obs.lib
    )
endif()

target_link_libraries(${PROJECT_NAME}
        ${CMAKE_JS_LIB}
        ${OBS_NODE_DEPS}
)