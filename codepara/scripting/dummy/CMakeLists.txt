cmake_minimum_required(VERSION 2.8.8)

# Make sure the user is not executing this script directly
if(NOT InOpenJK)
	message(FATAL_ERROR "Use the top-level cmake script!")
endif(NOT InOpenJK)

set(PSEName "PSE_dummy_${Architecture}")

set(PSEDummyFiles
        "${ParaScriptingDir}/dummy/dummy.c"
	)
source_group("psedummy" FILES ${PSEDummyFiles})

add_library(${PSEName} SHARED ${PSEDummyFiles} ${ParaScriptingFiles})
if(NOT WIN32)
	# remove "lib" prefix for .so/.dylib files
	set_target_properties(${PSEName} PROPERTIES PREFIX "")
endif(NOT WIN32)

install(TARGETS ${PSEName} LIBRARY DESTINATION ${PSEName})

set_target_properties(${PSEName} PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "${ReleaseDefines}")
set_target_properties(${PSEName} PROPERTIES COMPILE_DEFINITIONS_MINSIZEREL "${ReleaseDefines}")
set_target_properties(${PSEName} PROPERTIES COMPILE_DEFINITIONS_RELEASE "${ReleaseDefines}")
set_target_properties(${PSEName} PROPERTIES COMPILE_DEFINITIONS_DEBUG "${DebugDefines}")
set_target_properties(${PSEName} PROPERTIES INCLUDE_DIRECTORIES "${ParaScriptingIncludeDirs}")
set_target_properties(${PSEName} PROPERTIES PROJECT_LABEL ${PSEName})

# no libraries used
if(PSEDummyLibs)
	target_link_libraries(${MPGame} ${PSEDummyLibs})
endif(PSEDummyLibs)
