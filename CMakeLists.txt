cmake_minimum_required(VERSION 3.3.2)
project(garuda)

set(metadataSdkDir "" CACHE PATH "Path to unpacked VMS Metadata SDK zip.")
if(metadataSdkDir STREQUAL "")
    set(metadataSdkDir ${CMAKE_CURRENT_LIST_DIR}/../..) #< Assume building samples inside the SDK.
    if(NOT EXISTS ${metadataSdkDir}/src/nx/sdk OR NOT EXISTS ${metadataSdkDir}/nx_kit/src/nx/kit)
        message(FATAL_ERROR "Define metadataSdkDir cache variable to point to the unzipped SDK.")
    endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Path to Garuda Include Directory
set(GARUDA_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include/garuda)
set(MEDIA_SERVER_PLUGIN_DIR /opt/networkoptix-metavms/mediaserver/bin/plugins)
set(CMAKE_PREFIX_PATH /opt/networkoptix-metavms/mediaserver/lib/cmake)

# OpenCV & CUDA
set(CUDA_TOOLKIT_ROOT_DIR "/usr/local/cuda")
find_package(CUDA 12 REQUIRED)
find_package(OpenCV REQUIRED)

if(UNIX)
    # In Linux, for the plugin .so library, set `rpath` to "$ORIGIN" and do not set `runpath`, thus
    # enabling the lookup of the dependencies in the plugin dir first.
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " -Wl,--disable-new-dtags")
endif()
set(CMAKE_SKIP_BUILD_RPATH ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
set(CMAKE_INSTALL_RPATH "$ORIGIN")

if(WIN32)
    string(APPEND CMAKE_CXX_FLAGS " /MP") #< Use all CPU cores by MSVC.
    # Do not create separate .pdb files for object files (workaround for mspdbsrv.exe bug).
    add_compile_options("/Z7")
endif()

if(WIN32)
    set(API_EXPORT_MACRO "__declspec(dllexport)")
else()
    set(API_EXPORT_MACRO "__attribute__((visibility(\"default\")))")
endif()

#--------------------------------------------------------------------------------------------------
# Define nx_kit lib, static.

set(nxKitLibraryType "STATIC" CACHE STRING "" FORCE)
set(nxKitWithTests "NO" CACHE STRING "" FORCE)

add_subdirectory(${metadataSdkDir}/nx_kit ${CMAKE_CURRENT_BINARY_DIR}/nx_kit)

#--------------------------------------------------------------------------------------------------
# Define nx_sdk lib, static, depends on nx_kit.

set(SDK_SRC_DIR ${metadataSdkDir}/src)
file(GLOB_RECURSE SDK_SRC CONFIGURE_DEPENDS ${SDK_SRC_DIR}/*)

add_library(nx_sdk STATIC ${SDK_SRC})
target_include_directories(nx_sdk PUBLIC ${SDK_SRC_DIR})
target_link_libraries(nx_sdk PRIVATE nx_kit)

target_compile_definitions(nx_sdk PRIVATE NX_PLUGIN_API=${API_EXPORT_MACRO}) #< for nxLibContext()

#--------------------------------------------------------------------------------------------------
# Object Detector Library, depends on opencv and cuda.

set(OBJECT_DETECTOR_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src/garuda/object_detection)
file(GLOB_RECURSE OBJECT_DETECTOR_SRC CONFIGURE_DEPENDS
    ${OBJECT_DETECTOR_SRC_DIR}/*)

add_library(object_detector SHARED ${OBJECT_DETECTOR_SRC})
target_include_directories(object_detector PRIVATE ${GARUDA_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS})
target_link_libraries(object_detector nx_kit nx_sdk ${OpenCV_LIBS})

#--------------------------------------------------------------------------------------------------
# Define wildlife_detection_geolocation_plugin lib, dynamic, depends on nx_kit and nx_sdk.

set(WILDLIFE_DETECTION_GEOL_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src/garuda/vms_server_plugins/analytics/wildlife_detection_geolocation_plugin)
file(GLOB_RECURSE WILDLIFE_DETECTION_GEOL_SRC CONFIGURE_DEPENDS
    ${WILDLIFE_DETECTION_GEOL_SRC_DIR}/*)
set(WILDLIFE_DETECTION_GEOL_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include/garuda/vms_server_plugins/analytics)

add_library(wildlife_detection_geolocation_plugin SHARED ${WILDLIFE_DETECTION_GEOL_SRC} ${OBJECT_DETECTOR_SRC})
add_dependencies(wildlife_detection_geolocation_plugin object_detector)
target_include_directories(wildlife_detection_geolocation_plugin PRIVATE ${WILDLIFE_DETECTION_GEOL_INCLUDE_DIR} 
                            ${GARUDA_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS})
target_link_libraries(wildlife_detection_geolocation_plugin PRIVATE nx_kit nx_sdk ${OpenCV_LIBS})

target_compile_definitions(wildlife_detection_geolocation_plugin
    PRIVATE NX_PLUGIN_API=${API_EXPORT_MACRO})

#--------------------------------------------------------------------------------------------------
# Executable to test object detection with sample image, depends on opencv and cuda.

add_executable(test_object_detection ${WILDLIFE_DETECTION_GEOL_SRC_DIR}/test_object_detection.cpp ${OBJECT_DETECTOR_SRC})
target_include_directories(test_object_detection PRIVATE ${WILDLIFE_DETECTION_GEOL_INCLUDE_DIR} 
    ${GARUDA_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS})
target_link_libraries(test_object_detection PRIVATE nx_kit nx_sdk ${OpenCV_LIBS})

# Install the final built plugin library to /opt/networkoptix-metavms/mediaserver/plugins
install(TARGETS wildlife_detection_geolocation_plugin
        DESTINATION ${MEDIA_SERVER_PLUGIN_DIR})