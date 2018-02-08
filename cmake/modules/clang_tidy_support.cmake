find_program(
	CLANG_TIDY_EXE
	NAMES "clang-tidy"
	DOC "Path to clang-tidy executable"
)

macro(set_clang_tidy_args ARGS_LIST)
	if(CLANG_TIDY_EXE)
		message(STATUS "===================================")
		message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
		message(STATUS "clang-tidy command line: ${ARGS_LIST}")
		message(STATUS "===================================")
		
		set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}"
				"${ARGS_LIST}"
				#"-p=${PROJECT_BINARY_DIR}/compile_commands.json"
			)
		set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
		set(CMAKE_CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
	endif()
endmacro()
