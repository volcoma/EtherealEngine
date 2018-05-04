include(CMakeParseArguments)

function(copy_folder_to_runtime_path)
    cmake_parse_arguments(
        FUNCTION_ARGS
        ""
        "DESTINATION"
        "PATH;CONFIGURATIONS"
        ${ARGN}
    )

    get_filename_component(FULL_SOURCE_PATH "${FUNCTION_ARGS_PATH}" ABSOLUTE)

    file(GLOB_RECURSE FILES_TO_COPY
        RELATIVE "${FULL_SOURCE_PATH}"
        "${FULL_SOURCE_PATH}/*"
    )

    set(BIN_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

    # Copying of the files.
    set(DESTINATION "${BIN_PATH}/${FUNCTION_ARGS_DESTINATION}")
	
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${DESTINATION}")
    foreach (FILENAME ${FILES_TO_COPY})
        get_filename_component(FULL_DESTINATION_PATH "${DESTINATION}/${FILENAME}" DIRECTORY)
        execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${FULL_DESTINATION_PATH}")
        execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FULL_SOURCE_PATH}/${FILENAME}" "${FULL_DESTINATION_PATH}")
    endforeach ()
endfunction ()

function (copy_files_to_runtime_path)
    cmake_parse_arguments(
        FUNCTION_ARGS
        ""
        "DESTINATION"
        "FILES;CONFIGURATIONS"
        ${ARGN}
    )

    # Argument validation.
    if(NOT FUNCTION_ARGS_FILES)
        message(FATAL_ERROR "No files provided for copying.")
    endif()

    # Use default values if applicable.
    set(FUNCTION_ARGS_DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

    # Copying of the files.
    set(DESTINATION "${FUNCTION_ARGS_DESTINATION}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${DESTINATION}")
    foreach (FILENAME ${FUNCTION_ARGS_FILES})
        execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FILENAME}" "${DESTINATION}/")
    endforeach ()

endfunction ()
