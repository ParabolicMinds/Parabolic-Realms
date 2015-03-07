cmake_minimum_required(VERSION 2.8.8)

# Make sure the user is not executing this script directly
if(NOT InOpenJK)
	message(FATAL_ERROR "Use the top-level cmake script!")
endif(NOT InOpenJK)

#    Add PSE Projects
if(BuildScriptingLibraries)
	add_subdirectory("${ParaDir}/scripting")
endif(BuildScriptingLibraries)
