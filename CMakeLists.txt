cmake_minimum_required(VERSION 2.8.7)

# Allow use of project folders for IDEs like Visual Studio, so we
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

project(jubilant-funicular)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SKIP_RULE_DEPENDENCY TRUE)

option(NTA_USE_DEVIL "Turn off to use CImg instead of DevIL for image loading" ON)
option(NTA_USE_AUDIO "Turn off if you don't need AudioManager or SDL2_mixer" ON)
option(NTA_USE_IMGUI "Turn on if you want to use Imgui" OFF)	
option(NTA_BUILD_EXAMPLES "Turn off if you don't want to build the examples" OFF)
option(NTA_BUILD_TESTS "Turn off if you don't want to build the tests" ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(UTILS_BUILD_TESTS ${NTA_BUILD_TESTS} CACHE BOOL "" FORCE)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

add_subdirectory(utilities)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(NTA_LIBRARIES nta CACHE INTERNAL "")
# TODO (?): Include deps one at a time instead of all at once (e.g. optionally include imgui)
set(NTA_INCLUDE_DIR
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/deps
  ${UTILS_INCLUDE_DIR}
	${GLEW_INCLUDE_DIR}
	${GLUT_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIR}
	${SDL2_INCLUDE_DIR}
	${IL_INCLUDE_DIR}
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
  ${UTILS_LIBRARIES}
  ${SDL2_LIBRARY}
  ${SDL2_TTF_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${GLUT_LIBRARIES}
  ${OPENGL_LIBRARIES}
	CACHE INTERNAL ""
)

if(${NTA_BUILD_TESTS})
    message(STATUS "Building tests (run with make test)")
    enable_testing()
    add_subdirectory(tests)
endif()

if(${NTA_BUILD_EXAMPLES})
    message(STATUS "Building examples (find in examples_bin folder)")
    add_subdirectory(examples)
endif()
