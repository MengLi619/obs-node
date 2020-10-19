cmake_minimum_required(VERSION 3.13)
project(libobs_node)

set(CMAKE_CXX_STANDARD 14)

# Variables
if (NOT OBS_STUDIO_DIR)
    set(OBS_STUDIO_DIR libobs/dist)
endif()

message(STATUS "'OBS_STUDIO_DIR' = ${OBS_STUDIO_DIR}")

# Currently not only Apple is supported, other platform will add soon.
IF(NOT APPLE)
    message(FATAL_ERROR "Currently only Apple platform is supported.")
    return()
ENDIF()

# Build
file(GLOB SOURCE_FILES "src/addon/*.cpp" "src/addon/*.h")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
)
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

#set(CMAKE_BUILD_RPATH "@loader_path/")
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_JS_INC}
        ${NODE_ADDON_API_DIR}
        ${OBS_STUDIO_DIR}/include/
)

LIST(APPEND LIBOBS_LIBRARIES
    ${OBS_STUDIO_DIR}/bin/libobs.dylib
)

# Use executable path as rpath
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,@loader_path/")

target_link_libraries(${PROJECT_NAME}
        ${CMAKE_JS_LIB}
        ${LIBOBS_LIBRARIES}
)