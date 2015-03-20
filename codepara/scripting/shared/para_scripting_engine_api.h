#ifndef PARA_SCRIPTING_ENGINE_API_H
#define PARA_SCRIPTING_ENGINE_API_H

#include "pse_types.h"

#ifdef __cplusplus
	#if (defined _MSC_VER)
		#define Q_EXPORT extern "C" __declspec(dllexport)
	#elif (defined __SUNPRO_C)
		#define Q_EXPORT extern "C" __global
	#elif ((__GNUC__ >= 3) && (!__EMX__) && (!sun))
		#define EXPORT extern "C" __attribute__((visibility("default")))
	#else
		#define Q_EXPORT extern "C"
	#endif
#else
	#if (defined _MSC_VER)
		#define Q_EXPORT __declspec(dllexport)
	#elif (defined __SUNPRO_C)
		#define Q_EXPORT __global
	#elif ((__GNUC__ >= 3) && (!__EMX__) && (!sun))
		#define EXPORT __attribute__((visibility("default")))
	#else
		#define Q_EXPORT
	#endif
#endif

/*
================================================================
============================ NOTES =============================
================================================================

1.	PSE_Init and PSE_Identify are the only two functions plugins
		are REQUIRED to implement.

2.	Do not save returned pointers:   <-------- HEY YOU, READ THIS, IT'S VERY VERY IMPORTANT.
		(void *, char *, char const *)
		It's not even remotely safe.
	If you need them for later, make your own copy.
	If making a copy is not an option (void *),
		work around it, but do NOT keep it.
	In other words, always assume you're dealing with
		volatile memory, because you are in many cases.

3.	Do not use the Export functions in a separate thread,
		they are not even remotely thread-safe.
		(unless otherwise noted.)

4.	PSE_OpenManifest should return a very unique instance ID.
	If it's something like 1, 2, 3, etc., there will
		potentially be collision with other plugins.
	A good idea (and also the reason tyhe type is void *),
		is to return a pointer to the actual internal
		instance object.

================================================================
*/

typedef struct pseExport_s {
	void			(*Printf)			( char const *, ... );
	char *			(*Format)			( char const *, ... );

	//Info
	char const *	(*GetClientName)	( client_t handle );
	int				(*GetClientNum)		( client_t handle );

	//Action
	void			(*Say)				( char const * name, char const * msg );
	void			(*KillPlayer)		( client_t handle );
} pseExport_t;

/*
================================================================
======================== IMPLEMENT THESE =======================
================================================================
Parabolic Realms expects these functions to be present
in this library.
*/

//REQUIRED
EXPORT void			PSE_Init(pseExport_t pse); //Save this parameter, you need it to communicate with the Parabolic Realms runtime.
EXPORT char const *	PSE_Identify(); //Short (one word) string. Named on manifests to be loaded by the proper plugin.
EXPORT void *		PSE_OpenManifest(char const * manifest); //Create instances here, return a unique identifier that the CloseManifest function will recognize. Return 0 as a failure state.
EXPORT void			PSE_CloseManifest(void * uid);
EXPORT void			PSE_Shutdown();

//OPTIONAL
EXPORT void			PSE_Event_Frame(int levelTime);
EXPORT void			PSE_Event_ChatMsg(client_t clinum, char const * msg);

/*
================================================================
*/

#endif
