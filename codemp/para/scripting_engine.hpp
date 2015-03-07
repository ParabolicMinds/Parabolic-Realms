#ifndef SCRIPTING_ENGINE_HPP
#define SCRIPTING_ENGINE_HPP

#include "qcommon/qcommon.h"

#include "scripting_engine_vm.h"

// Client/Server
void PARA_ScriptingInit();

// VM
void * PARA_LoadManifest(char const * path);
void PARA_CloseManifest(void *);
para_seImport_t * PARA_GenerateImport();
void PARA_DeleteImport(para_seImport_t *);

// PSE
void PSE_Outgoing_Ping();
void PSE_Outgoing_ChatMsg(char const * name, char const * msg);

#endif //SCRIPTING_ENGINE_HPP
