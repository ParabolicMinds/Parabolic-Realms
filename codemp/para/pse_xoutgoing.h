#include "para/para_xcommon.h"

#ifdef _XPSEIMPORTDEF
#define XPSE_FUNC_1( rtype, name, a0t ) rtype ( * name ) ( a0t );
#define XPSE_FUNC_2( rtype, name, a0t, a1t ) rtype ( * name ) ( a0t, a1t );
#endif

#ifdef _XPSEFUNCDEF
#define XPSE_FUNC_1( rtype, name, a0t ) rtype PSE_##name ( a0t _a_A ) { for (pseImport_t * psel : pse_libs) { psel-> name ( _a_A ); } }
#define XPSE_FUNC_2( rtype, name, a0t, a1t ) rtype PSE_##name ( a0t _a_A, a1t _a_B ) { for (pseImport_t * psel : pse_libs) { psel-> name ( _a_A, _a_B ); } }
#endif

#ifdef _XPSEFUNCDEFPROT
#define XPSE_FUNC_1( rtype, name, a0t ) rtype PSE_##name ( a0t );
#define XPSE_FUNC_2( rtype, name, a0t, a1t ) rtype PSE_##name ( a0t, a1t );
#endif

#ifdef _XPSEFUNCASSIGN
#define XPSE_FUNC_1( rtype, name, a0t ) pse -> name = PSE_##name;
#define XPSE_FUNC_2( rtype, name, a0t, a1t ) pse -> name = PSE_##name;
#endif

#ifdef _XPSESYSLOAD
#define XPSE_FUNC_1( rtype, name, a0t ) pse -> name = Sys_LoadFunction(handle, QUOTE( PSE_##name )); if (! pse -> name ) goto psefail;
#define XPSE_FUNC_2( rtype, name, a0t, a1t ) pse -> name = Sys_LoadFunction(handle, QUOTE( PSE_##name )); if (! pse -> name ) goto psefail;
#endif

XPSE_FUNC_2 ( void, Event_ChatMsg, int32_t, char const * )

#undef XPSE_FUNC_1
#undef XPSE_FUNC_2
