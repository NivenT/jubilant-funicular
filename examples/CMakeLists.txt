SUBDIRLIST(DIRS ${CMAKE_CURRENT_SOURCE_DIR})
foreach(dir ${DIRS})
	file(GLOB_RECURSE SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${dir}/*.cpp)

	message(STATUS "${dir} -- ${SRCS}")

	add_executable(${dir} ${SRCS})
    target_link_libraries(${dir} ${NTA_LIBRARIES})
    target_include_directories(${dir} PUBLIC ${NTA_INCLUDE_DIR})

    # Keep examples in a examples_bin folder
    set_target_properties(${dir} PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
    	${CMAKE_CURRENT_BINARY_DIR}/../examples_bin/${dir})

    # This is kinda trash
    file(GLOB_RECURSE HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${dir}/*.h)
    file(GLOB_RECURSE RSRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${dir}/*)
    list(REMOVE_ITEM RSRCS ${SRCS} ${HDRS})
    foreach(rsrc ${RSRCS})
    	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${rsrc}
    		${CMAKE_CURRENT_BINARY_DIR}/../examples_bin/${rsrc} COPYONLY)
    endforeach(rsrc)
endforeach(dir)