#include "scripting_engine.hpp"
#include "sys/sys_loadlib.h"

#include <vector>

extern "C" {
#include "pse_loader.h"
}

std::vector<pseImport_t *> pse_libs;

void PARA_ScriptingInit() {
	Com_Printf("\n----- Initializing Scripting Engines -----\n");

	int fnum;
	char * * files = FS_ListFiles("para-plugins/", ".so", &fnum);
	for (int i = 0; i < fnum; i++) {
		if (!strncmp("PSE_", files[i], 4)) {
			pseImport_t * pse = PSE_LoadLibrary(va("base/para-plugins/%s", files[i]));
			if (!pse) continue;
			pse_libs.push_back(pse);
			Com_Printf("Loaded: %s\n", pse->Identify());
		}
	}

	Com_Printf("\n------------------------------------------\n\n");
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
	delete reinterpret_cast<pseOutgoingImport_t *>(import);
}

pseOutgoingImport_t * PARA_GenerateImport() {
	pseOutgoingImport_t * pse = new pseOutgoingImport_t();
#define _XPSEFUNCASSIGN
#include "pse_xoutgoing.h"
#undef _XPSEFUNCASSIGN
	return pse;
}

void PARA_DeleteImport(pseOutgoingImport_t * pse) {
	delete pse;
}

#define _XPSEFUNCDEF
#include "pse_xoutgoing.h"
#undef _XPSEFUNCDEF
