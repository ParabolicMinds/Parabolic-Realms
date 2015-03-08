#include "para/para_xcommon.h"

#ifdef _XFUNCDEF
#define XPSE_FUNC_1( rtype, name, rname, a0t, a0n ) rtype PSE_Incoming_##name ( a0t a0n ) { g_pse_e -> PSE_Incoming_##name( a0n ); }
#define XPSE_FUNC_2( rtype, name, rname, a0t, a0n, a1t, a1n ) rtype PSE_Incoming_##name ( a0t a0n, a1t a1n ) { g_pse_e -> PSE_Incoming_##name( a0n, a1n ); }
#endif

#ifdef _XFUNCDEFPROT
#define XPSE_FUNC_1( rtype, name, rname, a0t, a0n ) rtype PSE_Incoming_##name ( a0t a0n );
#define XPSE_FUNC_2( rtype, name, rname, a0t, a0n, a1t, a1n ) rtype PSE_Incoming_##name ( a0t a0n, a1t a1n );
#endif

#ifdef _XPSEEXPORTDEF
#define XPSE_FUNC_1( rtype, name, rname, a0t, a0n ) rtype ( *PSE_Incoming_##name ) ( a0t a0n );
#define XPSE_FUNC_2( rtype, name, rname, a0t, a0n, a1t, a1n ) rtype ( *PSE_Incoming_##name ) ( a0t a0n, a1t a1n );
#endif

#ifdef _XPSEEXPORTASSIGN
#define XPSE_FUNC_1( rtype, name, rname, a0t, a0n ) psex.name = PSE_Incoming_##name;
#define XPSE_FUNC_2( rtype, name, rname, a0t, a0n, a1t, a1n ) psex.name = PSE_Incoming_##name;
#endif

#ifdef _XPSEGAMEEXPORTASSIGN
#define XPSE_FUNC_1( rtype, name, rname, a0t, a0n ) pseie -> PSE_Incoming_##name = rname;
#define XPSE_FUNC_2( rtype, name, rname, a0t, a0n, a1t, a1n ) pseie -> PSE_Incoming_##name = rname;
#endif

XPSE_FUNC_2 ( void, Say, G_PSE_Say, char const *, name, char const *, msg )

#undef XPSE_FUNC_1
#undef XPSE_FUNC_2
