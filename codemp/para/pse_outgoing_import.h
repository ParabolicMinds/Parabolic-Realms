#ifndef PSE_OUTGOING_IMPORT_H
#define PSE_OUTGOING_IMPORT_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct pseOutgoingImport_s {
#define _XPSEIMPORTDEF
#include "para/pse_xoutgoing.h"
#undef _XPSEIMPORTDEF
} pseOutgoingImport_t;

#ifdef __cplusplus
}
#endif
#endif
