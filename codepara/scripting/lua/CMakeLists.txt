cmake_minimum_required(VERSION 2.8.8)

# Make sure the user is not executing this script directly
if(NOT InOpenJK)
	message(FATAL_ERROR "Use the top-level cmake script!")
endif(NOT InOpenJK)

set(PSEPluginShort "lua")

set(PSEName "PSE_${PSEPluginShort}_${Architecture}")

set(PSEPluginFiles
	"${ParaScriptingDir}/${PSEPluginShort}/core.cpp"
	)
source_group("pse${PSEPluginShort}" FILES ${PSEPluginFiles})

add_library(${PSEName} SHARED ${PSEPluginFiles} ${ParaScriptingFiles})

set_target_properties(${PSEName} PROPERTIES PREFIX "")

target_include_directories(${PSEName} PUBLIC ${ParaScriptingIncludeDirs})

target_link_libraries(${PSEName} "-llua")

set_target_properties(${PSEName} PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "${ReleaseDefines}")
set_target_properties(${PSEName} PROPERTIES COMPILE_DEFINITIONS_MINSIZEREL "${ReleaseDefines}")
set_target_properties(${PSEName} PROPERTIES COMPILE_DEFINITIONS_RELEASE "${ReleaseDefines}")
set_target_properties(${PSEName} PROPERTIES COMPILE_DEFINITIONS_DEBUG "${DebugDefines}")
set_target_properties(${PSEName} PROPERTIES PROJECT_LABEL ${PSEName})
install(
    TARGETS ${PSEName}
    LIBRARY DESTINATION "./base/para-plugins/"
)
