#ifndef BG_PHYS_H
#define BG_PHYS_H

#include "qcommon/q_shared.h"

void BG_InitializeSimulation();
void BG_ShutdownSimulation();
void BG_RunSimulation(); //parameters?
void BG_RegisterBPhysEntity(entityState_t * ent);

#endif //BG_PHYS_H
