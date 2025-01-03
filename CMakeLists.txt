cmake_minimum_required(VERSION 3.0.0)
project(RoboVis 
    VERSION 0.1.0
    DESCRIPTION "Robot Visualization"
    LANGUAGES CXX)

# ----------------------------------
# submodules
# ----------------------------------

# glm
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glm)

# glfw
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glfw)

# assimp
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/assimp)

# # imgui
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/imgui)

# ----------------------------------
# preparations
# ----------------------------------

# set cpp standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set output dir
set(OUTPUT_DIR ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
if(UNIX)	
    # update output dir
    set(OUTPUT_DIR ${OUTPUT_DIR}/$<CONFIGURATION>)
endif()

# set cmake output dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${OUTPUT_DIR})

# set sources and headers
file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)
file(GLOB SOURCES_IMGUI ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/imgui/*.cpp)
file(GLOB HEADERS_IMGUI ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/imgui/*.h)
set(SOURCES_IMGUI_BACKENDS ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/imgui/backends/imgui_impl_opengl3.cpp ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/imgui/backends/imgui_impl_glfw.cpp)
set(HEADERS_IMGUI_BACKENDS ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/imgui/backends/imgui_impl_opengl3.h ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/imgui/backends/imgui_impl_glfw.h)
set(SOURCE_STB ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/stb_image/stb_image.cpp)
set(HEADER_STB ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/stb_image/stb_image.h)

# set include directories
set(INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/imgui ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/stb_image)

# set linked libraries
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
set(LIBS Threads::Threads glm glfw GLESv2 assimp)

# set defines
set(DEFINES GLFW_INCLUDE_ES3 IMGUI_IMPL_OPENGL_ES3 IMGUI_DEFINE_MATH_OPERATORS GLM_ENABLE_EXPERIMENTAL)

# set precompiled header file
set(PCH ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h)

# set compile options
set(OPTIONS 
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>) #-Wpedantic

# ----------------------------------
# configure library
# ----------------------------------

# executable
add_executable(RoboVis ${SOURCES} ${HEADERS} ${SOURCES_IMGUI} ${HEADERS_IMGUI} ${SOURCES_IMGUI_BACKENDS} ${HEADERS_IMGUI_BACKENDS} ${SOURCE_STB} ${HEADER_STB})

# defines
target_compile_definitions(RoboVis PRIVATE ${DEFINES})

# include directories
target_include_directories(RoboVis PRIVATE ${INCLUDES})

# linked libraries
target_link_libraries(RoboVis PRIVATE ${LIBS}) 

# compile options
target_compile_options(RoboVis PRIVATE ${OPTIONS})

# precompiled headers
target_precompile_headers(RoboVis PRIVATE ${PCH})

# runtime library
if(MSVC)
	set_property(TARGET RoboVis PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
