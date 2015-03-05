#include "scripting_engine.hpp"

class seInstanceInterface {
public:
	seInstanceInterface();
	void Test() {Com_Printf("This is a test.\n");}
};

void PARA_ScriptingInit() {
	Com_Printf("----- Initializing Scripting Engines -----\n");
	Com_Printf("\n------------------------------------------\n");
}

constexpr char const * const para_script_dir = "para-scripts/";

void * PARA_LoadManifest(char const * path) {
	fileHandle_t fh = 0;
	char * srcpath = new char[strlen(para_script_dir) + strlen(path) + 1];
	strcpy(srcpath, para_script_dir);
	strcpy(srcpath + strlen(para_script_dir), path);
	if (!FS_FOpenFileRead(srcpath, &fh, qfalse)) return 0;
	delete [] srcpath;
	FS_FCloseFile(fh);
	return new char;
}

void PARA_CloseManifest(void * import) {
	delete reinterpret_cast<para_seImport_t *>(import);
}

para_seImport_t * PARA_GenerateImport() {
	para_seImport_t * pse = new para_seImport_t();
	pse->Test = Test;
	return pse;
}

void PARA_DeleteImport(para_seImport_t * pse) {
	delete pse;
}

void Test() {
	Com_Printf("...TESTING...\n");
}
