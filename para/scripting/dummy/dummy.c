#include "para_scripting_engine_api.h"

static pseExport_t psex;

void PSE_Init(pseExport_t pse) {
	psex = pse;
}

char const * PSE_Identify() {
	return "dummy";
}

void PSE_Ping() {
	psex.Printf("PONG\n");
}

void PSE_Event_ChatMsg(char const * name, char const * msg) {
	psex.Say("Dumbass", psex.Format("Shut the fuck up, %s, you dummy.\n", name));
}
