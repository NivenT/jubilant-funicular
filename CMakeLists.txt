cmake_minimum_required(VERSION 2.8.7)

# Allow use of project folders for IDEs like Visual Studio, so we
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

project(jubilant-funicular)

if(NOT ${CMAKE_VERSION} LESS 3.2)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
else()
    message(STATUS "Checking compiler flags for C++14 support.")
    # Set C++11/14 support flags for various compilers
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-std=c++14" COMPILER_SUPPORTS_CXX14)
    check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
    check_cxx_compiler_flag("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    if(COMPILER_SUPPORTS_CXX14)
        message(STATUS "C++14 is supported.")
        if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
        endif()
    elseif(COMPILER_SUPPORTS_CXX11)
        message(STATUS "C++11 is supported.")
        if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        endif()
    elseif(COMPILER_SUPPORTS_CXX0X)
        message(STATUS "C++0x is supported.")
        if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -stdlib=libc++")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
        endif()
    else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} does not have recent enough C++ support. Please use a different C++ compiler.")
    endif()
endif()

option(NTA_USE_DEVIL "Turn off to use CImg instead of DevIL for image loading" ON)
option(NTA_USE_AUDIO "Turn off if you don't need AudioManager or SDL2_mixer" ON)
option(NTA_USE_IMGUI "Turn on if you want to use Imgui" OFF)	

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(NTA_LIBRARIES nta CACHE INTERNAL "")
# TODO (?): Add include dirs from required packages
# TODO (?): Include deps one at a time instead of all at once (e.g. optionally include imgui)
set(NTA_INCLUDE_DIR
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/deps
	CACHE INTERNAL "")

if(${NTA_USE_DEVIL})
	add_definitions(-DNTA_USE_DEVIL) # #define NTA_USE_DEVIL
elseif()
	# TODO: Use cimg_plugin to get decent png support
endif()

if(${NTA_USE_AUDIO})
    add_definitions(-DNTA_USE_AUDIO) # #define NTA_USE_AUDIO
endif()

if(${NTA_USE_IMGUI})
    add_definitions(-DNTA_USE_IMGUI)
endif()

add_subdirectory(src)

if(${NTA_USE_DEVIL})
	message(STATUS "Using DevIL for image loading")
 	find_package(DevIL REQUIRED)
	set(NTA_LIBRARIES ${NTA_LIBRARIES} ${IL_LIBRARIES} ${ILU_LIBRARIES} CACHE INTERNAL "")
else()
	message(STATUS "Using CImg for image loading")
	# X11 is for CImg <--- TODO: This is OS dependent so make it OS agnostic instead
	set(NTA_LIBRARIES ${NTA_LIBRARIES} X11 CACHE INTERNAL "")
endif()
if(${NTA_USE_AUDIO})
    message(STATUS "Using SDL2_mixer for audio")
    find_package(SDL2_mixer REQUIRED)
    set(NTA_LIBRARIES ${NTA_LIBRARIES} ${SDL2_MIXER_LIBRARIES} CACHE INTERNAL "")
else()
    message(STATUS "Audio disabled")
endif()
if(${NTA_USE_IMGUI})
    message(STATUS "Using Imgui")
    add_library(imgui deps/imgui/imgui.cpp
                      deps/imgui/imgui_draw.cpp
                      deps/imgui/imgui_impl_sdl_gl3.cpp
                      deps/imgui/imgui_tabs.cpp)
    set(NTA_LIBRARIES ${NTA_LIBRARIES} imgui CACHE INTERNAL "")
else()
    message(STATUS "Not using Imgui")
endif()

# Is it standard practice to bundle libraries I don't own like this?
set(NTA_LIBRARIES ${NTA_LIBRARIES}
  ${SDL2_LIBRARY}
  ${SDL2_TTF_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${GLUT_LIBRARIES}
  ${OPENGL_LIBRARIES}
	CACHE INTERNAL ""
)
