#include "para/para_cvar_def.h"

#if defined(_CGAME)
#ifdef XCVAR_PROTO
	#define XCVAR_DEF( name, defVal, update, flags, announce ) extern vmCvar_t name;
#endif

#ifdef XCVAR_DECL
	#define XCVAR_DEF( name, defVal, update, flags, announce ) vmCvar_t name;
#endif

#ifdef XCVAR_LIST
	#define XCVAR_DEF( name, defVal, update, flags, announce ) { & name , #name , defVal , update , flags },
#endif

#elif defined(_GAME)
#ifdef XCVAR_PROTO
	#define XCVAR_DEF( name, defVal, update, flags, announce ) extern vmCvar_t name;
#endif

#ifdef XCVAR_DECL
	#define XCVAR_DEF( name, defVal, update, flags, announce ) vmCvar_t name;
#endif

#ifdef XCVAR_LIST
	#define XCVAR_DEF( name, defVal, update, flags, announce ) { & name , #name , defVal , update , flags , announce },
#endif
#endif

#define PCVAR_CG_TTS cg_tts

#if defined(_GAME) || defined(_CGAME) //Game and CGame cvars (bg_)
#endif

#if defined(_GAME) //Game cvars (g_)
#endif

#if defined(_CGAME) //CGame cvars (cg_)
XCVAR_DEF( PCVAR_CG_TTS,				"1",					NULL,					CVAR_ARCHIVE,				qfalse )
#endif

#undef XCVAR_DEF
