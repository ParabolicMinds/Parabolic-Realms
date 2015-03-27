extern "C" {
#include "bg_public.h"
#include "bg_phys.h"
}

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
} bulletEntity_t;

static bool init;
static btBroadphaseInterface * broadphase;
static btDefaultCollisionConfiguration * config;
static btCollisionDispatcher * dispatcher;
static btSequentialImpulseConstraintSolver * solver;
static btDiscreteDynamicsWorld * bworld;

static std::vector<bulletObject_t *> map_statics;
static std::vector<bulletEntity_t> active_states;

static int cm_brushes, cm_brushsides, cm_planes;

#define BP_POINTS_SIZE 512

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
constexpr int substep {80};
constexpr btScalar sim_step {1.0f / substep};
void BG_Run_Simulation() {
	while (run_sim) {
		if (run_advance) {
			sim_lock.lock();
			bworld->stepSimulation(run_advance.exchange(0) / 1000.0f, substep, sim_step);
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

void BG_InitializeSimulation() {
	if (init) return;

	Com_Printf("Physics: Initializing Simulation...\n");

	broadphase = new btDbvtBroadphase;
	config = new btDefaultCollisionConfiguration;
	dispatcher = new btCollisionDispatcher (config);
	solver = new btSequentialImpulseConstraintSolver;
	bworld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);

	bworld->setGravity(btVector3(0, 0, -400));

	init = true;

	trap->CM_NumData(&cm_brushes, &cm_brushsides, &cm_planes);

	vec3_t points[BP_POINTS_SIZE];
	Com_Printf("Physics: Adding Brushes...\n");
	for (int i = 0; i < cm_brushes; i++) {
		if (trap->CM_BrushContentFlags(i) & CONTENTS_SOLID) {
			int num = trap->CM_CalculateHull(i, points, BP_POINTS_SIZE);
			map_statics.push_back(B_CreateMapObject(points, num));
		}
	}

	run_sim.store(true);
	sim_thread = new std::thread(BG_Run_Simulation);
}

void BG_ShutdownSimulation() {
	if (!init) return;

	run_sim.store(false);
	if (sim_thread->joinable()) sim_thread->join();
	delete sim_thread; sim_thread = nullptr;

	Com_Printf("Shutting Down Physics Simulation...\n");

	for (bulletObject_t * obj : map_statics) {
		B_DeleteObject(obj);
	}
	map_statics.clear();

	for (bulletEntity_t & es : active_states) {
		B_ConfigureStateRem(es.ent);
		B_DeleteObject(es.physobj);
	}
	active_states.clear();

	delete bworld; bworld = nullptr;
	delete solver; solver = nullptr;
	delete dispatcher; dispatcher = nullptr;
	delete config; config = nullptr;
	delete broadphase; broadphase = nullptr;
	init = false;
}

constexpr float amult {57.2957795f};
void BG_UpdatePhysicsObjects() {
	if (!init) return;

	//sim_lock.lock();
	int i = 0;
	for (bulletEntity_t & bent : active_states) {
		i++;
		btTransform trans;
		bent.physobj->motionState->getWorldTransform(trans);
		btVector3 pos = trans.getOrigin();
		VectorSet(bent.ent->origin, pos.x(), pos.y(), pos.z());
		VectorSet(bent.ent->pos.trBase, pos.x(), pos.y(), pos.z());
		btMatrix3x3 rotmat {trans.getRotation()};
		rotmat.getEulerYPR(bent.ent->apos.trBase[1], bent.ent->apos.trBase[0], bent.ent->apos.trBase[2]);
		VectorScale(bent.ent->apos.trBase, amult, bent.ent->apos.trBase);
		VectorCopy(bent.ent->apos.trBase, bent.ent->angles);
		bent.ent->pos.trType = TR_STATIONARY;
		bent.ent->apos.trType = TR_STATIONARY;
	}
	//sim_lock.unlock();
}

void BG_RegisterBPhysSphereEntity(entityState_t * ent, float radius = 50.0f) {
	if (!init) return;

	auto iterator = active_states.begin();
	for (size_t i = 0; i < active_states.size(); i++, iterator++) {
		if ((*iterator).ent == ent) {
			return;
		}
	}
	B_ConfigureStateAdd(ent);
	active_states.push_back( {B_CreateSphereObject(ent, 1, radius), ent} );
}

void BG_RegisterBPhysModelHullEntity(entityState_t * ent, char const * model) {
	if (!init) return;

	auto iterator = active_states.begin();
	for (size_t i = 0; i < active_states.size(); i++, iterator++) {
		if ((*iterator).ent == ent) {
			return;
		}
	}
	B_ConfigureStateAdd(ent);
	active_states.push_back( {B_CreateModelHullObject(ent, -1, model), ent} );
}

void BG_UnregisterBPhysEntity(entityState_t * ent) {
	if (!init) return;

	auto iterator = active_states.begin();
	for (size_t i = 0; i < active_states.size(); i++, iterator++) {
		if ((*iterator).ent == ent) {
			B_ConfigureStateRem(ent);
			B_DeleteObject((*iterator).physobj);
			active_states.erase(iterator);
			break;
		}
	}
}

void BG_ApplyImpulse(entityState_t * ent, vec3_t impulse) {
	for (bulletEntity_t & bent : active_states) {
		if (bent.ent == ent) {
			bent.physobj->rigidBody->applyImpulse({impulse[0] * bent.physobj->mass, impulse[1] * bent.physobj->mass, impulse[2] * bent.physobj->mass}, {0, 0, 0});
			break;
		}
	}
}
