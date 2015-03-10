#ifndef PARA_SCRIPTING_ENGINE_API_H
#define PARA_SCRIPTING_ENGINE_API_H

#include "qcommon/q_shared.h"

typedef int clientNum_t;

typedef struct pseExport_s {
	void			(*Printf)			( char const *, ... );
	char *			(*Format)			( char const *, ... ); //WARNING: Make a copy if you intend to keep the result. The returned pointer is on VOLATILE MEMORY.

	void			(*Say)				( char const * name, char const * msg );
	char const *	(*GetClientName)	( clientNum_t num );
} pseExport_t;

/*
================================================================
======================== IMPLEMENT THESE =======================
================================================================
Parabolic Realms expects these functions to be present
in this library.
*/

Q_EXPORT void			PSE_Init(pseExport_t pse); //Save this parameter, you need it to communicate with the Parabolic Realms runtime.
Q_EXPORT char const *	PSE_Identify();

Q_EXPORT void			PSE_Event_ChatMsg(clientNum_t clinum, char const * msg);

/*
================================================================
*/

#endif
