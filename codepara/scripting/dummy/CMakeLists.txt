cmake_minimum_required(VERSION 2.8.8)

# Make sure the user is not executing this script directly
if(NOT InOpenJK)
	message(FATAL_ERROR "Use the top-level cmake script!")
endif(NOT InOpenJK)

set(PSEPluginShort "dummy")

set(PSEName "PSE_${PSEPluginShort}_${Architecture}")

set(PSEPluginFiles
	"${ParaScriptingDir}/${PSEPluginShort}/dummy.c"
	)
source_group("pse${PSEPluginShort}" FILES ${PSEPluginFiles})

add_library(${PSEName} SHARED ${PSEPluginFiles} ${ParaScriptingFiles})
if(NOT WIN32)
	# remove "lib" prefix for .so/.dylib files
	set_target_properties(${PSEName} PROPERTIES PREFIX "")
endif(NOT WIN32)

set_target_properties(${PSEName} PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "${ReleaseDefines}")
set_target_properties(${PSEName} PROPERTIES COMPILE_DEFINITIONS_MINSIZEREL "${ReleaseDefines}")
set_target_properties(${PSEName} PROPERTIES COMPILE_DEFINITIONS_RELEASE "${ReleaseDefines}")
set_target_properties(${PSEName} PROPERTIES COMPILE_DEFINITIONS_DEBUG "${DebugDefines}")
set_target_properties(${PSEName} PROPERTIES INCLUDE_DIRECTORIES "${ParaScriptingIncludeDirs}")
set_target_properties(${PSEName} PROPERTIES PROJECT_LABEL ${PSEName})
install(
    TARGETS ${PSEName}
    LIBRARY DESTINATION "./base/para-plugins/"
)

# no libraries used
if(PSEPluginLibs)
	target_link_libraries(${PSEName} ${PSEPluginLibs})
endif(PSEPluginLibs)
