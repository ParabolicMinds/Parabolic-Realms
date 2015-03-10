#ifndef PSE_INCOMING_EXPORT_H
#define PSE_INCOMING_EXPORT_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct pseIncomingExport_s {
#define _XPSEEXPORTDEF
#include "para/pse_xincoming.h"
#undef _XPSEEXPORTDEF
} pseIncomingExport_t;

#if !defined(_GAME)
extern pseIncomingExport_t * g_pse_e;
#endif

#ifdef __cplusplus
}
#endif
#endif
