extern "C" {
#include "bg_phys.h"
}
#include <btBulletDynamicsCommon.h>

static btBroadphaseInterface* broadphase;

void BG_InitializeSimulation() {
	if (!broadphase) broadphase = new btDbvtBroadphase();
}

void BG_ShutdownSimulation() {
	if(broadphase) { delete broadphase; broadphase = nullptr; }
}

void BG_RunSimulation() {

}

void BG_RegisterBPhysEntity(entityState_t * ent) {

}
