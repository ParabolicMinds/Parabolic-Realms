extern "C" {
#include "bg_public.h"
#include "bg_phys.h"
}

#include <vector>

#include <btBulletDynamicsCommon.h>

typedef struct bulletObject_s {
	btScalar									mass;
	btVector3									inertia;
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

static btCollisionShape * testPlane;
static btDefaultMotionState * testPlaneState;
static btRigidBody::btRigidBodyConstructionInfo * testPlaneRigidInfo;
static btRigidBody * testPlaneRigidBody;

static int cm_brushes, cm_brushsides, cm_planes;

#define BP_POINTS_SIZE 512

static void B_ConfigureStateAdd(entityState_t * ent) {
	ent->eFlags |= EF_BULLET_PHYS;
}

static void B_ConfigureStateRem(entityState_t * ent) {
	ent->eFlags &= ~EF_BULLET_PHYS;
}

static bulletObject_t * B_CreateObject(entityState_t * ent, btScalar mass = 0) {
	bulletObject_t * obj = new bulletObject_t;
	obj->mass = mass;
	obj->inertia = {0, 0, 0};
	obj->shape = new btSphereShape(1);
	obj->motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3 {ent->origin[0], ent->origin[1], ent->origin[2]}));
	obj->shape->calculateLocalInertia(mass, obj->inertia);
	obj->CI = new btRigidBody::btRigidBodyConstructionInfo(mass, obj->motionState, obj->shape, obj->inertia);
	obj->rigidBody = new btRigidBody(*obj->CI);
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

void BG_InitializeSimulation() {
	if (init) return;

	Com_Printf("Starting Physics Simulation...\n");

	broadphase = new btDbvtBroadphase;
	config = new btDefaultCollisionConfiguration;
	dispatcher = new btCollisionDispatcher (config);
	solver = new btSequentialImpulseConstraintSolver;
	bworld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);

	bworld->setGravity(btVector3(0, 0, -10));

	/*
	testPlane = new btStaticPlaneShape (btVector3(0, 0, 1), 1);
	testPlaneState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, -1)));
	testPlaneRigidInfo = new btRigidBody::btRigidBodyConstructionInfo(0, testPlaneState, testPlane, btVector3(0, 0, 0));
	testPlaneRigidBody = new btRigidBody(*testPlaneRigidInfo);
	bworld->addRigidBody(testPlaneRigidBody);
	*/

	init = true;

	trap->CM_NumData(&cm_brushes, &cm_brushsides, &cm_planes);

	for (int i = 0; i < cm_brushes; i++) {
		if (trap->CM_BrushContentFlags(i) & CONTENTS_SOLID) {
			Com_Printf("Adding Brush %i...\n", i);
			vec3_t points[BP_POINTS_SIZE];
			int num = trap->CM_CalculateHull(i, points, BP_POINTS_SIZE);
			map_statics.push_back(B_CreateMapObject(points, num));
		}
	}
}

void BG_ShutdownSimulation() {
	if (!init) return;

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

//static int leveltimeprev = 0;

static btScalar simStep {1.0f / 40.0f};

constexpr float amult {57.2957795f};

static int lastleveltime = 0;
void BG_RunSimulation(int leveltime) {
	if (!init) return;

	float adv = (leveltime - lastleveltime) / 1000.0f;
	bworld->stepSimulation(adv, 4, simStep);
	lastleveltime = leveltime;

	int i = 0;
	for (bulletEntity_t & bent : active_states) {
		i++;
		btTransform trans;
		bent.physobj->motionState->getWorldTransform(trans);
		VectorSet(bent.ent->origin, trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z());
		VectorSet(bent.ent->pos.trBase, trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z());
		btMatrix3x3 rotmat {trans.getRotation()};
		rotmat.getEulerYPR(bent.ent->apos.trBase[1], bent.ent->apos.trBase[0], bent.ent->apos.trBase[2]);
		VectorScale(bent.ent->apos.trBase, amult, bent.ent->apos.trBase);
		VectorCopy(bent.ent->apos.trBase, bent.ent->angles);
		bent.ent->pos.trType = TR_STATIONARY;
		bent.ent->apos.trType = TR_STATIONARY;
	}
}

void BG_RegisterBPhysEntity(entityState_t * ent) {
	if (!init) return;

	auto iterator = active_states.begin();
	for (size_t i = 0; i < active_states.size(); i++, iterator++) {
		if ((*iterator).ent == ent) {
			return;
		}
	}

	B_ConfigureStateAdd(ent);
	active_states.push_back( {B_CreateObject(ent, 1), ent} );
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
			bent.physobj->rigidBody->applyImpulse({impulse[0], impulse[1], impulse[2]}, {0, 0, 0});
			break;
		}
	}
}
