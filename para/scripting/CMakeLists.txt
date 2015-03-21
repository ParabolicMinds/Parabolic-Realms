cmake_minimum_required(VERSION 2.8.8)

# Make sure the user is not executing this script directly
if(NOT InOpenJK)
	message(FATAL_ERROR "Use the top-level cmake script!")
endif(NOT InOpenJK)

set(ParaScriptingDir "${ParaDir}/scripting")
set(ParaScriptingIncludeDirs "${ParaScriptingDir}/shared")
set(ParaScriptingIncludeDirs ${ParaScriptingIncludeDirs} "${MPDir}")

option(BuildDummyLibrary "Whether to build the dummy (test) library." ON)
if(BuildDummyLibrary)
	add_subdirectory("${ParaScriptingDir}/dummy")
endif(BuildDummyLibrary)
