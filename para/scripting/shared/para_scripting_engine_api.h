#ifndef PARA_SCRIPTING_ENGINE_API_H
#define PARA_SCRIPTING_ENGINE_API_H

#include "qcommon/q_shared.h"

typedef struct pseExport_s {
	void			(*Printf)			( char const *, ... );
	void			(*Say)				( char const * name, char const * msg );
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
Q_EXPORT void			PSE_Ping();
Q_EXPORT void			PSE_Event_ChatMsg(char const * name, char const * msg);

/*
================================================================
*/

#endif
