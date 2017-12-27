# \brief adds for the given target a fake target which allows all
#        headers and symbols to be shown in IDEs.
# \param target_name Which target properties should be added to the IDE support target.
function(target_add_ide_support target_name)
  if (NOT TARGET ${target_name})
    message(FATAL_ERROR "No target defined with name ${target_name}, cannot target_add_ide_support it.")
  endif()

  set (target_for_ide "${target_name}_ide")
  if (NOT TARGET ${target_for_ide})
      file(GLOB_RECURSE target_for_ide_srcs "*.h" "*.hpp" "*.hxx" "*.c" "*.cpp" "*.cxx" "*.cc")
      add_custom_target(${target_for_ide} SOURCES ${target_for_ide_srcs})
  endif()

endfunction(target_add_ide_support)
