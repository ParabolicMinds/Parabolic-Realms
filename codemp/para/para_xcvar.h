#include "para/para_cvar_def.h"
#include "para/para_xcommon.h"

#if defined(_CGAME)
#ifdef XCVAR_PROTO
	#define XCVAR_DEF( name, defVal, update, flags, announce ) extern vmCvar_t name;
#endif

#ifdef XCVAR_DECL
	#define XCVAR_DEF( name, defVal, update, flags, announce ) vmCvar_t name;
#endif

#ifdef XCVAR_LIST
	#define XCVAR_DEF( name, defVal, update, flags, announce ) { & name , QUOTE(name) , defVal , update , flags },
#endif
#endif
#if defined(_GAME)
#ifdef XCVAR_PROTO
	#define XCVAR_DEF( name, defVal, update, flags, announce ) extern vmCvar_t name;
#endif

#ifdef XCVAR_DECL
	#define XCVAR_DEF( name, defVal, update, flags, announce ) vmCvar_t name;
#endif

#ifdef XCVAR_LIST
	#define XCVAR_DEF( name, defVal, update, flags, announce ) { & name , QUOTE(name) , defVal , update , flags , announce },
#endif
#endif

#if defined(_GAME) || defined(_CGAME) //Game and CGame cvars (bg_)
XCVAR_DEF( PCVAR_BG_INFFORCE,			"0",							NULL,										CVAR_ARCHIVE|CVAR_SYSTEMINFO,				qtrue )
#endif

#if defined(_GAME) //Game cvars (g_)
XCVAR_DEF( PCVAR_G_BODYTIMEMULT,		"1.0",							NULL,										CVAR_ARCHIVE,				qtrue )
XCVAR_DEF( PCVAR_G_GOLF,				"0",							G_PCVAR_GOLF_Switchfunc,					CVAR_ARCHIVE,				qtrue )
#endif

#if defined(_CGAME) //CGame cvars (cg_)
XCVAR_DEF( PCVAR_CG_SPRAYSHADER,		"textures/imperial/basic",		CG_PCVAR_SPRAYSHADER_Setfunc,				CVAR_ARCHIVE,				qfalse )
XCVAR_DEF( PCVAR_CG_TTS,				"0",							NULL,										CVAR_ARCHIVE,				qfalse )
#endif

#undef XCVAR_DEF