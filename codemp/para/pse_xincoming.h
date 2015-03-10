#include "para/para_xcommon.h"

#ifdef _XFUNCDEF
#define XPSE_FUNC_1( rtype, name, a0t ) rtype PSE_Incoming_##name ( a0t _a_A ) { return g_pse_e -> PSE_Incoming_##name( _a_A ); }
#define XPSE_FUNC_2( rtype, name, a0t, a1t ) rtype PSE_Incoming_##name ( a0t _a_A, a1t _a_B ) { return g_pse_e -> PSE_Incoming_##name( _a_A, _a_B ); }
#endif

#ifdef _XFUNCDEFPROT
#define XPSE_FUNC_1( rtype, name, a0t ) rtype PSE_Incoming_##name ( a0t );
#define XPSE_FUNC_2( rtype, name, a0t, a1t ) rtype PSE_Incoming_##name ( a0t, a1t );
#endif

#ifdef _XPSEEXPORTDEF
#define XPSE_FUNC_1( rtype, name, a0t ) rtype ( *PSE_Incoming_##name ) ( a0t );
#define XPSE_FUNC_2( rtype, name, a0t, a1t ) rtype ( *PSE_Incoming_##name ) ( a0t, a1t );
#endif

#ifdef _XPSEEXPORTASSIGN
#define XPSE_FUNC_1( rtype, name, a0t ) psex.name = PSE_Incoming_##name;
#define XPSE_FUNC_2( rtype, name, a0t, a1t ) psex.name = PSE_Incoming_##name;
#endif

#ifdef _XPSEGAMEEXPORTASSIGN
#define XPSE_FUNC_1( rtype, name, a0t ) pseie -> PSE_Incoming_##name = G_PSE_##name;
#define XPSE_FUNC_2( rtype, name, a0t, a1t ) pseie -> PSE_Incoming_##name = G_PSE_##name;
#endif

//Actions
XPSE_FUNC_2 ( void, Say, char const *, char const * ) //Name, Message

//Getters
XPSE_FUNC_1 ( char const *, GetClientName, int ) //Client Num

#undef XPSE_FUNC_1
#undef XPSE_FUNC_2
