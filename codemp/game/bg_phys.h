#ifndef BG_PHYS_H
#define BG_PHYS_H

#include "qcommon/q_shared.h"
#include "g_local.h"

typedef enum physShape_s {
	BPHYS_SPHERE,
	BPHYS_CUBE
} physShape_t;

void BG_InitializeSimulation();
void BG_ShutdownSimulation();
void BG_RunSimulation(int leveltime);
void BG_RegisterBPhysEntity(entityState_t * ent);
void BG_UnregisterBPhysEntity(entityState_t * ent);

#endif //BG_PHYS_H
