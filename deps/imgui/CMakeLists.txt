cmake_minimum_required(VERSION 2.8.7)

# Allow use of project folders for IDEs like Visual Studio, so we
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

project(jubilant-funicular)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SKIP_RULE_DEPENDENCY TRUE)

#set(CMAKE_CXX_FLAGS -Wfatal-errors ${CMAKE_CXX_FLAGS})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(IMGUI_LIBRARIES imgui CACHE INTERNAL "")
set(IMGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")

add_library(imgui ${CMAKE_CURRENT_SOURCE_DIR}/imgui.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/imgui_draw.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/imgui_demo.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_opengl3.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_sdl.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/imguidatechooser.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/imguifilesystem.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/imguinodegrapheditor.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/imguistring.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/imgui_widgets.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/minizip/ioapi.c
                  ${CMAKE_CURRENT_SOURCE_DIR}/minizip/unzip.c
                  ${CMAKE_CURRENT_SOURCE_DIR}/minizip/zip.c)
target_include_directories(imgui PUBLIC ${IMGUI_INCLUDE_DIR})
