cmake_minimum_required(VERSION 2.8.8)

# Make sure the user is not executing this script directly
if(NOT InOpenJK)
	message(FATAL_ERROR "Use the top-level cmake script!")
endif(NOT InOpenJK)

set(ParaScriptingIncludeDirs "${ParaDir}" "${MPDir}" "${ParaPSESharedDir}")
set(ParaScriptingDir "${ParaDir}/scripting")
set(ParaScriptingFiles
    "${ParaPSESharedDir}/para_scripting_engine_api.h"
    "${ParaPSESharedDir}/pse_types.h"
    )

option(BuildDummyLibrary "Whether to build the dummy (test) library." ON)
if(BuildDummyLibrary)
        add_subdirectory("${ParaScriptingDir}/dummy")
endif(BuildDummyLibrary)
