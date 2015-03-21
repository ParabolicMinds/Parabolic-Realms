#include "para_scripting_engine_api.h"

static pseExport_t psex;

void PSE_Init(pseExport_t pse) {
	psex = pse;
}

char const * PSE_Identify() {
	return "dummy";
}

void * PSE_OpenManifest(char const * manifest) {
	psex.Printf(manifest);
	return 0;
}

void PSE_CloseManifest(void * uid) {

}

void PSE_Shutdown() {

}

void PSE_Event_ChatMsg(client_t client, char const * msg) {
	psex.Say("Dummy", psex.Format("Shut the fuck up, %s.\n", psex.GetClientName(client)));
}
