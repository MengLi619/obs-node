cmake_minimum_required(VERSION 3.13)
project(libobs_node)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-DNAPI_VERSION=3)

## Detect Libary Suffix
#IF(WIN32)
#    SET(libExtension ".dll")
#ELSEIF(APPLE)
#    SET(libExtension ".dylib")
#ELSE()
#    SET(libExtension ".so")
#ENDIF()

# Build a shared library named after the project from the files in `src/`
file(GLOB SOURCE_FILES "src/*.cpp" "src/*.h")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})

# Gives our library file a .node extension without any "lib" prefix
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# Essential include files to build a node addon,
# You should add this line in every CMake.js based project
# Include N-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
)
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_JS_INC}
        ${NODE_ADDON_API_DIR}
        ${OBS_STUDIO_DIR}/libobs/
        ${OBS_STUDIO_DIR}/deps/
)

# Libobs dependecies
#IF(WIN32)
#    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
#    LIST(APPEND node-obs_THIRDPARTY
#        "${obsBuildPath}/libobs/RelWithDebInfo/obs${libExtension}"
#        "${obsBuildPath}/libobs/RelWithDebInfo/obs.pdb"
#        "${obsBuildPath}/libobs-d3d11/RelWithDebInfo/libobs-d3d11${libExtension}"
#        "${obsBuildPath}/libobs-d3d11/RelWithDebInfo/libobs-d3d11.pdb"
#        "${obsBuildPath}/libobs-opengl/RelWithDebInfo/libobs-opengl${libExtension}"
#        "${obsBuildPath}/libobs-opengl/RelWithDebInfo/libobs-opengl.pdb"
#        "${obsBuildPath}/deps/glad/RelWithDebInfo/obsglad${libExtension}"
#        "${obsBuildPath}/deps/w32-pthreads/RelWithDebInfo/w32-pthreads${libExtension}"
#        "${obsBuildPath}/rundir/RelWithDebInfo/bin/${BITS}bit/avcodec-57${libExtension}"
#        "${obsBuildPath}/rundir/RelWithDebInfo/bin/${BITS}bit/avdevice-57${libExtension}"
#        "${obsBuildPath}/rundir/RelWithDebInfo/bin/${BITS}bit/avfilter-6${libExtension}"
#        "${obsBuildPath}/rundir/RelWithDebInfo/bin/${BITS}bit/avformat-57${libExtension}"
#        "${obsBuildPath}/rundir/RelWithDebInfo/bin/${BITS}bit/avutil-55${libExtension}"
#        "${obsBuildPath}/rundir/RelWithDebInfo/bin/${BITS}bit/libcurl${libExtension}"
#        "${obsBuildPath}/rundir/RelWithDebInfo/bin/${BITS}bit/libogg-0${libExtension}"
#        "${obsBuildPath}/rundir/RelWithDebInfo/bin/${BITS}bit/libopus-0${libExtension}"
#        "${obsBuildPath}/rundir/RelWithDebInfo/bin/${BITS}bit/libvorbis-0${libExtension}"
#        "${obsBuildPath}/rundir/RelWithDebInfo/bin/${BITS}bit/libvorbisenc-2${libExtension}"
#        "${obsBuildPath}/rundir/RelWithDebInfo/bin/${BITS}bit/libvorbisfile-3${libExtension}"
#        "${obsBuildPath}/rundir/RelWithDebInfo/bin/${BITS}bit/libvpx-1${libExtension}"
#        "${obsBuildPath}/rundir/RelWithDebInfo/bin/${BITS}bit/libx264-148${libExtension}"
#        "${obsBuildPath}/rundir/RelWithDebInfo/bin/${BITS}bit/swresample-2${libExtension}"
#        "${obsBuildPath}/rundir/RelWithDebInfo/bin/${BITS}bit/swscale-4${libExtension}"
#        "${obsBuildPath}/rundir/RelWithDebInfo/bin/${BITS}bit/w32-pthreads${libExtension}"
#        "${obsBuildPath}/rundir/RelWithDebInfo/bin/${BITS}bit/zlib${libExtension}"
#        "${PROJECT_SOURCE_DIR}/resources/roboto.png"
#    )
#ELSEIF(APPLE)
#    LIST(APPEND node-obs_THIRDPARTY
#        "${obsBuildPath}/libobs/RelWithDebInfo/libobs.0${libExtension}"
#        "${obsBuildPath}/libobs-opengl/RelWithDebInfo/libobs-opengl.so"
#        "${obsBuildPath}/rundir/RelWithDebInfo/bin/libobsglad.0${libExtension}"
#    )
#ELSEIF("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
#    LIST(APPEND node-obs_THIRDPARTY
#        "${obsBuildPath}/libobs/RelWithDebInfo/libsobs.0${libExtension}"
#        "${obsBuildPath}/libobs-opengl/RelWithDebInfo/libobs-opengl${libExtension}"
#    )
#ENDIF()

target_link_libraries(${PROJECT_NAME}
        ${CMAKE_JS_LIB}
)