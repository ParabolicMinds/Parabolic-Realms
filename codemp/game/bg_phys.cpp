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

static std::vector<bulletEntity_t> active_states;

static btCollisionShape * testPlane;
static btDefaultMotionState * testPlaneState;
static btRigidBody::btRigidBodyConstructionInfo * testPlaneRigidInfo;
static btRigidBody * testPlaneRigidBody;

static void B_ConfigureStateAdd(entityState_t * ent) {
	ent->eFlags |= EF_BULLET_PHYS;
}

static void B_ConfigureStateRem(entityState_t * ent) {
	ent->eFlags &= ~EF_BULLET_PHYS;
}

static bulletObject_t * B_CreateObject(btScalar mass = 0) {
	bulletObject_t * obj = new bulletObject_t;
	obj->mass = mass;
	obj->inertia = {0, 0, 0};
	obj->shape = new btSphereShape(1);
	obj->motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 50 )));
	obj->shape->calculateLocalInertia(mass, obj->inertia);
	obj->CI = new btRigidBody::btRigidBodyConstructionInfo(mass, obj->motionState, obj->shape, obj->inertia);
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

	testPlane = new btStaticPlaneShape (btVector3(0, 0, 1), 1);
	testPlaneState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, -1)));
	testPlaneRigidInfo = new btRigidBody::btRigidBodyConstructionInfo(0, testPlaneState, testPlane, btVector3(0, 0, 0));
	testPlaneRigidBody = new btRigidBody(*testPlaneRigidInfo);
	bworld->addRigidBody(testPlaneRigidBody);

	init = true;

	int a, b, c;
	trap->CM_NumData(&a, &b, &c);
	Com_Printf("Brushes: %i\nBrushsides: %i\nPlanes: %i\n", a, b, c);
}

void BG_ShutdownSimulation() {
	if (!init) return;

	Com_Printf("Shutting Down Physics Simulation...\n");

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

static btScalar simStep {1.0f / 120.0f};

void BG_RunSimulation(int leveltime) {
	if (!init) return;

	bworld->stepSimulation(1 / 120.f, 120, simStep);

	int i = 0;
	for (bulletEntity_t & bent : active_states) {
		i++;
		btTransform trans;
		bent.physobj->motionState->getWorldTransform(trans);
		VectorSet(bent.ent->origin, trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z());
		VectorSet(bent.ent->pos.trBase, trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z());
		bent.ent->pos.trType = TR_STATIONARY;
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
	active_states.push_back( {B_CreateObject(1), ent} );
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
