#ifndef SCRIPTING_ENGINE_HPP
#define SCRIPTING_ENGINE_HPP

#include "qcommon/qcommon.h"

#include "pse_outgoing_import.h"

// Client/Server
void PARA_ScriptingInit();
void PARA_ScriptingShutdown();

// VM
void * PARA_LoadManifest(char const * path);
void PARA_CloseManifest(void *);
pseOutgoingImport_t * PARA_GenerateImport();
void PARA_DeleteImport(pseOutgoingImport_t *);

#define _XPSEFUNCDEFPROT
#include "pse_xoutgoing.h"
#undef _XPSEFUNCDEFPROT

#endif //SCRIPTING_ENGINE_HPP
