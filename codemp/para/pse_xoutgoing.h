#include "para/para_xcommon.h"

#ifdef _XPSEIMPORTDEF
#define XPSE_FUNC_1( rtype, name, a0t ) rtype ( * name ) ( a0t );
#define XPSE_FUNC_2( rtype, name, a0t, a1t ) rtype ( * name ) ( a0t, a1t );
#endif

#ifdef _XPSEFUNCDEF
#define XPSE_FUNC_1( rtype, name, a0t ) rtype PSE_Internal_##name ( a0t _a_A ) { for (pseImport_t * psel : pse_libs) { if ( psel -> name ) psel-> name ( _a_A ); } }
#define XPSE_FUNC_2( rtype, name, a0t, a1t ) rtype PSE_Internal_##name ( a0t _a_A, a1t _a_B ) { for (pseImport_t * psel : pse_libs) { if ( psel -> name ) psel-> name ( _a_A, _a_B ); } }
#endif

#ifdef _XPSEFUNCDEFPROT
#define XPSE_FUNC_1( rtype, name, a0t ) rtype PSE_Internal_##name ( a0t );
#define XPSE_FUNC_2( rtype, name, a0t, a1t ) rtype PSE_Internal_##name ( a0t, a1t );
#endif

#ifdef _XPSEFUNCASSIGN
#define XPSE_FUNC_1( rtype, name, a0t ) pse -> name = PSE_Internal_##name;
#define XPSE_FUNC_2( rtype, name, a0t, a1t ) pse -> name = PSE_Internal_##name;
#endif

#ifdef _XPSESYSLOAD
#define XPSE_FUNC_1( rtype, name, a0t ) pse -> name = Sys_LoadFunction(handle, QUOTE( PSE_##name )); if (! pse -> name ) { Com_Printf("Warning: Unresolved Function: \"%s\"\n", QUOTE( PSE_##name ) ); }
#define XPSE_FUNC_2( rtype, name, a0t, a1t ) pse -> name = Sys_LoadFunction(handle, QUOTE( PSE_##name )); if (! pse -> name ) { Com_Printf("Warning: Unresolved Function: \"%s\"\n", QUOTE( PSE_##name ) ); }
#endif

XPSE_FUNC_1 ( void, Event_Frame, int )
XPSE_FUNC_2 ( void, Event_ChatMsg, void *, char const * )

#undef XPSE_FUNC_1
#undef XPSE_FUNC_2
