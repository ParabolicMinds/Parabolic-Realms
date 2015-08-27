#ifndef G_PARA_H
#define G_PARA_H

#include "g_local.h"

void G_PCVAR_GOLF_Switchfunc();

#define G_PSE_MAX_MANIFESTS 20

typedef enum {G_PSM_OK, G_PSM_FNF_NOPLUG, G_PSM_LIMIT} pse_loadstatus_t;

pse_loadstatus_t G_PSE_LoadManifest(char const * path);
void G_PSE_PurgeManifests();

#endif //G_PARA_H
