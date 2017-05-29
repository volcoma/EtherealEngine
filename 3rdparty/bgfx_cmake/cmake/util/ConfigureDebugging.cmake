# ConfigureDebugging.cmake
# Written in 2017 by Joshua Brookover <joshua.al.brookover@gmail.com>

# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

# You should have received a copy of the CC0 Public Domain Dedication along with
# this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
#
# configure_debugging( TARGET [OPTIONS...] )
#
# Configures the debugging settings in visual studio.
# Results in a no-op on non visual studio builds.
# Must be called in the same cmake file as the add_executable command.
#
# See OPTIONS variable in the function for supported user settings.
# See CONFIGS variable in the function for supported cmake configurations.
# See PROCESSORS variable in the function for supported architecture configurations.
#
# All variables can be set with one of the following formats:
#
# (OPTION)
# (OPTION)_(CONFIG)
# (OPTION)_(CONFIG)_(ARCH)
# (OPTION)_(ARCH)
#
# So, some examples (variables should be all caps):
#
# WORKING_DIR
# WORKING_DIR_X64
# WORKING_DIR_RELEASE_WIN32
# WORKING_DIR_X64
#
# An example of a full command:
#
# configure_debugging(target COMMAND "node.exe" COMMAND_X64 "node64.exe" WORKING_DIR ${CMAKE_SOURCE_DIR} DEBUGGER_ENV "PATH=%PATH%\\;$(ProjectDir)")

include( CMakeParseArguments )

function( configure_debugging ARG_TARGET )
	if( MSVC )
		# Visual Studio Options
		set(
			OPTIONS
			WORKING_DIR LocalDebuggerWorkingDirectory
			DEBUGGER_ENV LocalDebuggerEnvironment
			COMMAND LocalDebuggerCommand
			COMMAND_ARGS LocalDebuggerCommandArguments
		)

		# Valid Configurations
		set(
			CONFIGS
			Debug
			Release
			MinSizeRel
			RelWithDebInfo
		)

		# Processors
		set(
			PROCESSORS
			Win32
			x64
		)

		# Begin hackery
		if( ${CMAKE_SIZEOF_VOID_P} EQUAL 8 )
			set( ACTIVE_PROCESSOR "x64" )
		else()
			set( ACTIVE_PROCESSOR "Win32" )
		endif()
		# Fix issues with semicolons, thx cmake
		foreach( ARG ${ARGN} )
			string( REPLACE ";" "\\\\\\\\\\\\\\;" RES "${ARG}" )
			list( APPEND ARGS "${RES}" )
		endforeach()
		# Build options for cmake_parse_arguments, result is ONE_ARG variable
		set( ODD ON )
		foreach( OPTION ${OPTIONS} )
			if( ODD )
				set( ARG ${OPTION} )
				list( APPEND ONE_ARG ${ARG} )
				foreach( CONFIG ${CONFIGS} )
					string( TOUPPER ${CONFIG} CONFIG )
					list( APPEND ONE_ARG ${ARG}_${CONFIG} )
					foreach( PROCESSOR ${PROCESSORS} )
						string( TOUPPER ${PROCESSOR} PROCESSOR )
						list( APPEND ONE_ARG ${ARG}_${CONFIG}_${PROCESSOR} )
					endforeach()
				endforeach()
				foreach( PROCESSOR ${PROCESSORS} )
					string( TOUPPER ${PROCESSOR} PROCESSOR )
					list( APPEND ONE_ARG ${ARG}_${PROCESSOR} )
				endforeach()
				set( ODD OFF )
			else()
				set( ODD ON )
			endif()
		endforeach()
		cmake_parse_arguments( ARG "" "${ONE_ARG}" "" ${ARGS} )
		# Parse options, fills in all variables of format ARG_(ARG)_(CONFIG)_(PROCESSOR), for example ARG_WORKING_DIR_DEBUG_X64
		set( ODD ON )
		foreach( OPTION ${OPTIONS} )
			if( ODD )
				set( ARG ${OPTION} )
				foreach( CONFIG ${CONFIGS} )
					string( TOUPPER ${CONFIG} CONFIG_CAP )
					if( "${ARG_${ARG}_${CONFIG_CAP}}" STREQUAL "" )
						set( ARG_${ARG}_${CONFIG_CAP} ${ARG_${ARG}} )
					endif()
					foreach( PROCESSOR ${PROCESSORS} )
						string( TOUPPER ${PROCESSOR} PROCESSOR_CAP )
						if( "${ARG_${ARG}_${CONFIG_CAP}_${PROCESSOR_CAP}}" STREQUAL "" )
							if( "${ARG_${ARG}_${PROCESSOR_CAP}}" STREQUAL "" )
								set( ARG_${ARG}_${CONFIG_CAP}_${PROCESSOR_CAP} ${ARG_${ARG}_${CONFIG_CAP}} )
							else()
								set( ARG_${ARG}_${CONFIG_CAP}_${PROCESSOR_CAP} ${ARG_${ARG}_${PROCESSOR_CAP}} )
							endif()
						endif()
						if( NOT "${ARG_${ARG}_${CONFIG_CAP}_${PROCESSOR_CAP}}" STREQUAL "" )
						endif()
					endforeach()
				endforeach()
				set( ODD OFF )
			else()
				set( ODD ON )
			endif()
		endforeach()
		# Create string to put in proj.vcxproj.user file
		set( RESULT "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<Project ToolsVersion=\"12.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">" )
		foreach( CONFIG ${CONFIGS} )
			string( TOUPPER ${CONFIG} CONFIG_CAPS )
			foreach( PROCESSOR ${PROCESSORS} )
				if( "${PROCESSOR}" STREQUAL "${ACTIVE_PROCESSOR}" )
					string( TOUPPER ${PROCESSOR} PROCESSOR_CAPS )
					set( RESULT "${RESULT}\n  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='${CONFIG}|${PROCESSOR}'\">" )
					set( ODD ON )
					foreach( OPTION ${OPTIONS} )
						if( ODD )
							set( ARG ${OPTION} )
							set( ODD OFF )
						else()
							set( VALUE ${ARG_${ARG}_${CONFIG_CAPS}_${PROCESSOR_CAPS}} )
							if( NOT "${VALUE}" STREQUAL "" )
								set( RESULT "${RESULT}\n    <${OPTION}>${VALUE}</${OPTION}>" )
							endif()
							set( ODD ON )
						endif()
					endforeach()
					set( RESULT "${RESULT}\n  </PropertyGroup>" )
				endif()
			endforeach()
		endforeach()
		set( RESULT "${RESULT}\n</Project>" )
		file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/${ARG_TARGET}.vcxproj.user "${RESULT}" )
	endif()
endfunction()
