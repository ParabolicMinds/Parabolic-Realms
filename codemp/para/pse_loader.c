#include "pse_loader.h"
#include "pse_api.h"

#include "qcommon/q_shared.h"
#include "sys/sys_loadlib.h"

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
	pse->Ping = Sys_LoadFunction(handle, "PSE_Ping");
	if (!pse->Ping) goto psefail;
	pse->Event_ChatMsg = Sys_LoadFunction(handle, "PSE_Event_ChatMsg");
	if (!pse->Event_ChatMsg) goto psefail;

	pseExport_t psex;
	psex.Printf = Com_Printf;
	psex.Say	= PSE_Incomming_G_Say;
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


void PSE_Incomming_G_Say(char const * name, char const * msg) {
	//Invoke G Import Code...
}
