#ifndef PSE_LOADER_H
#define PSE_LOADER_H

#include "qcommon/q_shared.h"

#include "para_scripting_engine_api.h"

typedef struct pseImport_s {
	void *			handle;
	char const *	(*Identify)			( void );
	void *			(*OpenManifest)		( char const * );
	void			(*CloseManifest)	( void * );
	void			(*Shutdown)			( void );
#define _XPSEIMPORTDEF
#include "pse_xoutgoing.h"
#undef _XPSEIMPORTDEF
} pseImport_t;

pseImport_t * PSE_LoadLibrary(char * const path);
void PSE_UnloadLibrary(pseImport_t ** import);

#endif //PSE_LOADER_H
