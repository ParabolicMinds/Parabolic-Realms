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
XCVAR_DEF( PCVAR_BG_INFAMMO,			"0",							NULL,										CVAR_ARCHIVE|CVAR_SYSTEMINFO,				qtrue )
XCVAR_DEF( PCVAR_BG_INFFORCE,			"0",							NULL,										CVAR_ARCHIVE|CVAR_SYSTEMINFO,				qtrue )
XCVAR_DEF( PCVAR_BG_SABERLEN,			"1.0",							NULL,										CVAR_SYSTEMINFO,							qtrue )
XCVAR_DEF( PCVAR_BG_SUPERWEAPONS,		"1.0",							NULL,										CVAR_ARCHIVE|CVAR_SYSTEMINFO,				qtrue )
#endif

#if defined(_GAME) //Game cvars (g_)
XCVAR_DEF( PCVAR_G_BODYTIMEMULT,		"1.0",							NULL,										CVAR_ARCHIVE,				qtrue )
XCVAR_DEF( PCVAR_G_GHOOKDAMAGE,			"5",							NULL,										CVAR_ARCHIVE,				qtrue )
XCVAR_DEF( PCVAR_G_GHOOKDAMP,			"0.25",							NULL,										CVAR_ARCHIVE,				qtrue )
XCVAR_DEF( PCVAR_G_GHOOKSPEED,			"1000.0",						NULL,										CVAR_ARCHIVE,				qtrue )
XCVAR_DEF( PCVAR_G_GHOOKPULLSPEED,		"600.0",						NULL,										CVAR_ARCHIVE,				qtrue )
XCVAR_DEF( PCVAR_G_GOLF,				"0",							G_PCVAR_GOLF_Switchfunc,					CVAR_ARCHIVE,				qtrue )
XCVAR_DEF( PCVAR_G_SBDROPVALID,			"1",							NULL,										CVAR_ARCHIVE,				qtrue )
XCVAR_DEF( PCVAR_G_SPRAYMINHEIGHT,		"0.125",						NULL,										CVAR_ARCHIVE,				qtrue )
XCVAR_DEF( PCVAR_G_SPRAYMAXHEIGHT,		"8.0",							NULL,										CVAR_ARCHIVE,				qtrue )
XCVAR_DEF( PCVAR_G_SPRAYMINWIDTH,		"0.125",						NULL,										CVAR_ARCHIVE,				qtrue )
XCVAR_DEF( PCVAR_G_SPRAYMAXWIDTH,		"8.0",							NULL,										CVAR_ARCHIVE,				qtrue )
#endif

#if defined(_CGAME) //CGame cvars (cg_)
XCVAR_DEF( PCVAR_CG_TTS,				"0",							NULL,										CVAR_ARCHIVE,				qfalse )
#endif

#undef XCVAR_DEF
