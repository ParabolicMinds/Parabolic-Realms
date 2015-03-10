#include "pse_loader.h"
#include "pse_api.h"

#include "sys/sys_loadlib.h"

#include "pse_incoming_export.h"
pseIncomingExport_t * g_pse_e;

#include "scripting/shared/para_scripting_engine_api.h"

typedef void (*PSEInitFunc) ( pseExport_t );

pseImport_t * PSE_LoadLibrary(char * const path) {
	void * handle = Sys_LoadDll(path, qfalse);
	if (!handle) {
		Com_Printf("Could not load shared library at: \"%s\"\n", path);
		return 0;
	};
	pseImport_t * pse = calloc(1, sizeof(pseImport_t));
	pse->handle = handle;
	PSEInitFunc psei = Sys_LoadFunction(handle, "PSE_Init");
	if (!psei) goto psefail;
	pse->Identify = Sys_LoadFunction(handle, "PSE_Identify");
	if (!pse->Identify) goto psefail;
#define _XPSESYSLOAD
#include "pse_xoutgoing.h"
#undef _XPSESYSLOAD

	pseExport_t psex;
	psex.Printf = Com_Printf;
	psex.Format = va;
#define _XPSEEXPORTASSIGN
#include "para/pse_xincoming.h"
#undef _XPSEEXPORTASSIGN
	psei(psex);

	return pse;

	psefail:
	Com_Printf("Could not load scripting engine plugin due to unresolved API functions at: \"%s\"\n", path);
	Sys_UnloadLibrary(pse->handle);
	free(pse);
	return 0;
}

void PSE_UnloadLibrary(pseImport_t ** import) {
	Sys_UnloadLibrary((*import)->handle);
	free(*import);
	*import = 0;
}

#define _XFUNCDEF
#include "para/pse_xincoming.h"
#undef _XFUNCDEF
