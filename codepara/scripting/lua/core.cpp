#include "para_scripting_engine_api.h"

#include <lua.hpp>

#include <vector>

static pseExport_t psex;

EXPORT void PSE_Init(pseExport_t pse) {
	psex = pse;
}

EXPORT char const * PSE_Identify() {
	return "lua";
}

EXPORT void * PSE_OpenManifest(char const * manifest) {
	lua_State * state = luaL_newstate();
	luaL_openlibs(state);
	return state;
}

EXPORT void PSE_CloseManifest(void * uid) {
	lua_close((lua_State *)uid);
}

EXPORT void PSE_Shutdown() {
}

EXPORT void PSE_Event_ChatMsg(client_t client, char const * msg) {

}

EXPORT void PSE_Event_Frame(int levelTime) {

}
