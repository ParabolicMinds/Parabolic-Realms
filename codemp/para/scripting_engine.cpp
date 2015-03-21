#include "scripting_engine.hpp"
#include "sys/sys_loadlib.h"

#include <vector>
#include <unordered_map>

extern "C" {
#include "pse_loader.h"
}

std::vector<pseImport_t *> pse_libs;
std::unordered_map<void *, pseImport_t *> active_manifests;

void PARA_ScriptingInit() {
	Com_Printf("\n----- Initializing Scripting Engines -----\n");

	int fnum;
	char * * files = FS_ListFiles("para-plugins/", ".so", &fnum);

	for (int i = 0; i < fnum; i++) {
		if (!strncmp("PSE_", files[i], 4)) {
			pseImport_t * pse = PSE_LoadLibrary(va("base/para-plugins/%s", files[i]));
			if (!pse) continue;
			pse_libs.push_back(pse);
			Com_Printf("Successfully Loaded: %s\n", pse->Identify());

			if (!strcmp(pse->Identify(), "dummy")) {
				void * dummy_test = PARA_LoadManifest("dummy.psm");
				if (dummy_test) PARA_CloseManifest(dummy_test);
			}
		}
	}

	FS_FreeFileList(files);

	Com_Printf("\n------------------------------------------\n\n");
}

void PARA_ScriptingShutdown() {
	Com_Printf("\n----- Shutting Down Scripting Engines -----\n");

	if (active_manifests.size() > 0) {
		Com_Printf("Closing %i active manifests.\n", active_manifests.size());
		for(auto i : active_manifests) {
			i.second->CloseManifest(i.first);
		}
		active_manifests.clear();
	}

	for(auto i : pse_libs) {
		Com_Printf("Shutting down: \"%s\"\n", i->Identify());
		i->Shutdown();
	}
	pse_libs.clear();

	Com_Printf("\n-------------------------------------------\n\n");
}

constexpr char const * const para_script_dir = "para-scripts/";

void * PARA_LoadManifest(char const * path) {
	long size = 0;
	char * data;
	size = FS_ReadFile(va("%s%s", para_script_dir, path), reinterpret_cast<void * *>(&data));
	if (!size) {
		Com_Printf("Failed to read manifest at: \"%s\"\n", va("%s%s", para_script_dir, path));
	}

	if (data[0] != '#') {
		return NULL;
	}
	long i = 1;
	for (;i < size; i++) {
		switch (data[i]) {
		case '\0':
			break;
		case '\r':
			data[i] = '\0';
			continue;
		case '\n':
			data[i] = '\0';
			break;
		default:
			continue;
		}
		break;
	}

	void * handle;
	for (pseImport_t * pse : pse_libs) {
		if (!strcmp(data + 1, pse->Identify())) {
			handle = pse->OpenManifest(data + i + 1);
			if (handle) {
				active_manifests[handle] = pse;
			}
			break;
		}
	}

	FS_FreeFile(reinterpret_cast<void *>(data));
	return handle;
}

void PARA_CloseManifest(void * uid) {
	active_manifests[uid]->CloseManifest(uid);
	active_manifests.erase(uid);
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
