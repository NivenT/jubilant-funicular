file(GLOB_RECURSE TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach(test ${TEST_SRCS})
    # Get file name without extension
    get_filename_component(name ${test} NAME_WE)

    add_executable(${name} ${test})
    target_link_libraries(${name} ${NTA_LIBRARIES})
    target_include_directories(${name} PUBLIC ${NTA_INCLUDE_DIR})

    # Keep tests in a tests_bin folder
    set_target_properties(${name} PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/tests_bin)

    add_test(NAME ${name} 
                 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests_bin 
                 COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tests_bin/${name})
endforeach(test)