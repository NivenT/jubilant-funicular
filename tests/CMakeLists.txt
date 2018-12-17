file(GLOB_RECURSE TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
foreach(test ${TEST_SRCS})
    # Get file name without extension
    get_filename_component(name ${test} NAME_WE)
    get_filename_component(dir ${test} DIRECTORY)

    add_executable(${name} ${test})
    target_link_libraries(${name} ${NTA_LIBRARIES})
    target_include_directories(${name} PUBLIC ${NTA_INCLUDE_DIR})

    # Keep tests in a tests_bin folder
    set_target_properties(${name} PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/../tests_bin/${dir})

    add_test(NAME ${name} 
                 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../tests_bin/${dir} 
                 COMMAND ${CMAKE_CURRENT_BINARY_DIR}/../tests_bin/${dir}/${name})

    complement_file_list(RSRCS ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp" "*.h" "CMakeLists.txt")
    foreach(rsrc ${RSRCS})
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${rsrc}
            ${CMAKE_CURRENT_BINARY_DIR}/../tests_bin/${rsrc} COPYONLY)
    endforeach(rsrc)
endforeach(test)