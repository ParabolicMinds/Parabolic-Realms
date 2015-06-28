extern "C" {
#include "bg_public.h"
#include "bg_phys.h"
}

#define BP_USE_FIXEDPOINT_INT_32 1

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

#include <btBulletDynamicsCommon.h>

typedef struct bulletObject_s {
	btScalar									mass;
	btVector3									inertia;
	btVector3									mins;
	btVector3									maxs;
	btCollisionShape *							shape;
	btDefaultMotionState *						motionState;
	btRigidBody::btRigidBodyConstructionInfo *	CI;
	btRigidBody *								rigidBody;
} bulletObject_t;

typedef struct bulletEntity_s {
	bulletObject_t * physobj;
	entityState_t * ent;
	vec3_t stored_pos;
	vec3_t stored_ang;
} bulletEntity_t;

typedef struct bulletEntity2_s {
	bulletObject_t * physobj;
	gentity_t * gent;
	vec3_t stored_pos;
	vec3_t stored_ang;
} bulletEntity2_t;

static bool init;
static btBroadphaseInterface * broadphase;
static btDefaultCollisionConfiguration * config;
static btCollisionDispatcher * dispatcher;
static btSequentialImpulseConstraintSolver * solver;
static btDiscreteDynamicsWorld * bworld;

static std::vector<bulletObject_t *> map_statics {};
static std::vector<bulletEntity2_t> map_dynamics {};
static std::vector<bulletEntity_t> active_states {};

static int cm_brushes, cm_patches;

#define BP_POINTS_SIZE 2048

static void B_ConfigureStateAdd(entityState_t * ent) {
	ent->eFlags |= EF_BULLET_PHYS;
}

static void B_ConfigureStateRem(entityState_t * ent) {
	ent->eFlags &= ~EF_BULLET_PHYS;
}

static bulletObject_t * B_CreateSphereObject(entityState_t * ent, btScalar mass, btScalar radius) {
	bulletObject_t * obj = new bulletObject_t;
	obj->mass = mass;
	obj->inertia = {0, 0, 0};
	obj->shape = new btSphereShape(radius);
	obj->motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3 {ent->origin[0], ent->origin[1], ent->origin[2]}));
	obj->shape->calculateLocalInertia(mass, obj->inertia);
	obj->CI = new btRigidBody::btRigidBodyConstructionInfo(mass, obj->motionState, obj->shape, obj->inertia);
	obj->rigidBody = new btRigidBody(*obj->CI);
	bworld->addRigidBody(obj->rigidBody);
	return obj;
}

static bulletObject_t * B_CreateModelHullObject(entityState_t * ent, btScalar mass, char const * model) {
	bulletObject_t * obj = new bulletObject_t;
	obj->inertia = {0, 0, 0};
	vec3_t points[BP_POINTS_SIZE];
	int vnum = trap->CM_GetModelVerticies(model, points, BP_POINTS_SIZE);
	btConvexHullShape * chs = new btConvexHullShape();
	for (int i = 0; i < vnum; i++) {
		btVector3 cp {points[i][0], points[i][1], points[i][2]};
		if (i == 0) {
			obj->mins = cp;
			obj->maxs = cp;
		} else {
			obj->mins.setX(minimum(obj->mins.x(), cp.x()));
			obj->mins.setY(minimum(obj->mins.y(), cp.y()));
			obj->mins.setZ(minimum(obj->mins.z(), cp.z()));
			obj->maxs.setX(maximum(obj->maxs.x(), cp.x()));
			obj->maxs.setY(maximum(obj->maxs.y(), cp.y()));
			obj->maxs.setZ(maximum(obj->maxs.z(), cp.z()));
		}
		chs->addPoint(cp, false);
	}
	if (mass >= 0) obj->mass = mass;
	else {
		obj->mass = (obj->maxs.x() - obj->mins.x()) * (obj->maxs.y() - obj->mins.y()) * (obj->maxs.z() - obj->mins.z());
	}
	chs->recalcLocalAabb();
	obj->shape = chs;
	obj->motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3 {ent->origin[0], ent->origin[1], ent->origin[2]}));
	obj->shape->calculateLocalInertia(obj->mass, obj->inertia);
	obj->CI = new btRigidBody::btRigidBodyConstructionInfo(obj->mass, obj->motionState, obj->shape, obj->inertia);
	obj->rigidBody = new btRigidBody(*obj->CI);
	obj->rigidBody->setDamping(0.125f, 0.125f);
	bworld->addRigidBody(obj->rigidBody);
	return obj;
}

static bulletObject_t * B_CreateMapObject(vec3_t * points, int points_num) {
	bulletObject_t * obj = new bulletObject_t;
	obj->mass = 0;
	obj->inertia = {0, 0, 0};
	btConvexHullShape * chs = new btConvexHullShape();
	for (int i = 0; i < points_num; i++) {
		btVector3 vec {points[i][0], points[i][1], points[i][2]};
		chs->addPoint(vec, false);
	}
	chs->recalcLocalAabb();
	obj->shape = chs;
	obj->motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, -1 )));
	obj->CI = new btRigidBody::btRigidBodyConstructionInfo(obj->mass, obj->motionState, obj->shape, obj->inertia);
	obj->rigidBody = new btRigidBody(*obj->CI);
	bworld->addRigidBody(obj->rigidBody);
	return obj;
}

static void B_DeleteObject(bulletObject_t *& obj) {
	bworld->removeRigidBody(obj->rigidBody);
	delete obj->rigidBody;
	delete obj->CI;
	delete obj->motionState;
	delete obj->shape;
	delete obj;
	obj = nullptr;
}

// SIMULATION THREAD //
std::atomic_bool run_sim {false};
std::atomic_int run_advance {0};
std::mutex sim_lock;
#define SIM_SLEEP std::chrono::duration<float, std::ratio<1, 1000>>(1)
constexpr int substep {160};
constexpr btScalar sim_step {1.0f / substep};
void BG_Run_Simulation() {
	while (run_sim) {
		if (run_advance) {
			sim_lock.lock();
			bworld->stepSimulation(run_advance.exchange(0) / 1000.0f, substep, sim_step);
			for (bulletEntity2_t & bent : map_dynamics) {
				btTransform trans = bent.physobj->rigidBody->getWorldTransform();
				trans.setOrigin( {bent.stored_pos[0], bent.stored_pos[1], bent.stored_pos[2]} );
				bent.physobj->rigidBody->setWorldTransform(trans);
				//btMatrix3x3 rotmat {trans.getRotation()};
				//rotmat.getEulerYPR(bent.stored_ang[1], bent.stored_ang[0], bent.stored_ang[2]);
				//VectorScale(bent.stored_ang, 57.2957795f, bent.stored_ang);
			}
			for (bulletEntity_t & bent : active_states) {
				btTransform trans;
				bent.physobj->motionState->getWorldTransform(trans);
				btVector3 pos = trans.getOrigin();
				VectorSet(bent.stored_pos, pos.x(), pos.y(), pos.z());
				btMatrix3x3 rotmat {trans.getRotation()};
				rotmat.getEulerYPR(bent.stored_ang[1], bent.stored_ang[0], bent.stored_ang[2]);
				VectorScale(bent.stored_ang, 57.2957795f, bent.stored_ang);
			}
			sim_lock.unlock();
		}
		std::this_thread::sleep_for(SIM_SLEEP);
	}
}
std::thread * sim_thread {nullptr};

void BG_AdvanceSimulationTarget(int msec) {
	run_advance += msec;
}
//---------------------

std::atomic_int brush_index;
std::mutex stm;
std::vector<std::thread *> sb_workers;

// STATIC BRUSHES //
int * brushes = nullptr;
void BG_InitStaticBrushes_ThreadRun() {
	vec3_t points[BP_POINTS_SIZE];
	while (true) {
		int v = brush_index--;
		if (v < 0) break;
		v = brushes[v];
		if (trap->CM_BrushContentFlags(v) & CONTENTS_SOLID) {
			int num = trap->CM_CalculateHull(v, points, BP_POINTS_SIZE);
			stm.lock();
			bulletObject_t * bob = B_CreateMapObject(points, num);
			map_statics.push_back(bob);
			stm.unlock();
		}
	}
}

void BG_InitStaticBrushes() {
	bworld->setForceUpdateAllAabbs(false);
	brushes = new int [cm_brushes];
	brush_index = trap->CM_SubmodelIndicies(0, brushes) - 1;
	for (uint i = 0; i < std::thread::hardware_concurrency(); i++) {
		sb_workers.push_back(new std::thread(BG_InitStaticBrushes_ThreadRun));
	}
	for (std::thread * th : sb_workers) {
		if (th->joinable()) th->join();
		delete th;
	}
	delete [] brushes;
}
//---------------------

void BG_InitializeSimulation() {
	if (init) return;

	Com_Printf("Physics: Initializing Simulation...\n");

	broadphase = new btDbvtBroadphase();
	config = new btDefaultCollisionConfiguration;
	dispatcher = new btCollisionDispatcher (config);
	solver = new btSequentialImpulseConstraintSolver;
	bworld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);

	bworld->setGravity(btVector3(0, 0, -500));

	init = true;
}

void BG_InitializeSimulationStatics() {
	if (!init) return;

	trap->CM_NumData(&cm_brushes, &cm_patches);
	BG_InitStaticBrushes();
}

void BG_InitializeSimulationDynamics() {
	if (!init) return;
	/*
	gentity_t * gent = g_entities;
	for (int g = 0; g < MAX_GENTITIES; g++, gent++) {
		if (!gent->r.bmodel) continue;
		 int * gbrushes = new int [cm_brushes];
		 int gi = strtol(gent->model + 1, nullptr, 10);
		 assert(gi > 0);
		 int gbrushsize = trap->CM_SubmodelIndicies(gi, gbrushes);
		 if (gbrushsize > 0) {
			 vec3_t points[BP_POINTS_SIZE];
			 int num = trap->CM_CalculateHull(gbrushes[0], points, BP_POINTS_SIZE);
			 bulletEntity2_t bent {B_CreateMapObject(points, num), gent};
			 VectorCopy(bent.gent->s.origin, bent.stored_pos);
			 VectorCopy(bent.gent->s.angles, bent.stored_ang);
			 map_dynamics.push_back(bent);
		 }
		 delete [] gbrushes;
	}
	*/
}

void BG_StartSimulation() {
	if (!init) return;
	run_sim.store(true);
	sim_thread = new std::thread(BG_Run_Simulation);
}

void BG_ShutdownSimulation() {
	if (!init) return;

	run_sim.store(false);
	if (sim_thread->joinable()) sim_thread->join();
	delete sim_thread; sim_thread = nullptr;

	Com_Printf("Shutting Down Physics Simulation...\n");

	for (bulletEntity_t & es : active_states) {
		B_ConfigureStateRem(es.ent);
		B_DeleteObject(es.physobj);
	}
	active_states.clear();

	for (bulletEntity2_t & es : map_dynamics) {
		B_ConfigureStateRem(&es.gent->s);
		B_DeleteObject(es.physobj);
	}
	map_dynamics.clear();

	for (bulletObject_t * obj : map_statics) {
		B_DeleteObject(obj);
	}
	map_statics.clear();

	delete bworld; bworld = nullptr;
	delete solver; solver = nullptr;
	delete dispatcher; dispatcher = nullptr;
	delete config; config = nullptr;
	delete broadphase; broadphase = nullptr;
	init = false;
}

void BG_UpdatePhysicsObjects() {
	if (!init) return;

	sim_lock.lock();
	for (bulletEntity2_t & bent : map_dynamics) {
		VectorCopy(bent.gent->r.currentOrigin, bent.stored_pos);
		//VectorCopy(bent.gent->angles2, bent.stored_ang );
	}
	for (bulletEntity_t & bent : active_states) {
		VectorCopy(bent.stored_pos, bent.ent->pos.trBase);
		VectorCopy(bent.stored_pos, bent.ent->origin);
		VectorCopy(bent.stored_ang, bent.ent->apos.trBase);
		VectorCopy(bent.stored_ang, bent.ent->angles);
	}
	sim_lock.unlock();
}

void BG_RegisterBPhysSphereEntity(entityState_t * ent, float radius = 50.0f) {
	if (!init) return;

	sim_lock.lock();
	auto iterator = active_states.begin();
	for (size_t i = 0; i < active_states.size(); i++, iterator++) {
		if ((*iterator).ent == ent) {
			return;
		}
	}
	B_ConfigureStateAdd(ent);
	bulletEntity_t bent {B_CreateSphereObject(ent, 1, radius), ent};
	VectorCopy(bent.ent->origin, bent.stored_pos);
	VectorCopy(bent.ent->origin, bent.stored_ang);
	active_states.push_back( bent );
	sim_lock.unlock();
}

void BG_RegisterBPhysModelHullEntity(entityState_t * ent, char const * model) {
	if (!init) return;

	sim_lock.lock();
	auto iterator = active_states.begin();
	for (size_t i = 0; i < active_states.size(); i++, iterator++) {
		if ((*iterator).ent == ent) {
			return;
		}
	}
	B_ConfigureStateAdd(ent);
	bulletEntity_t bent {B_CreateModelHullObject(ent, -1, model), ent};
	VectorCopy(bent.ent->origin, bent.stored_pos);
	VectorCopy(bent.ent->origin, bent.stored_ang);
	active_states.push_back( bent );
	sim_lock.unlock();
}

void BG_UnregisterBPhysEntity(entityState_t * ent) {
	if (!init) return;

	sim_lock.lock();
	auto iterator = active_states.begin();
	for (size_t i = 0; i < active_states.size(); i++, iterator++) {
		if ((*iterator).ent == ent) {
			B_ConfigureStateRem(ent);
			B_DeleteObject((*iterator).physobj);
			active_states.erase(iterator);
			break;
		}
	}
	sim_lock.unlock();
}

void BG_ApplyImpulse(entityState_t * ent, vec3_t impulse) {
	sim_lock.lock();
	for (bulletEntity_t & bent : active_states) {
		if (bent.ent == ent) {
			bent.physobj->rigidBody->applyImpulse({impulse[0] * bent.physobj->mass, impulse[1] * bent.physobj->mass, impulse[2] * bent.physobj->mass}, {0, 0, 0});
			break;
		}
	}
	sim_lock.unlock();
}
