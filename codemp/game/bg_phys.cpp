extern "C" {
#include "bg_public.h"
#include "bg_phys.h"
}

#include <vector>

#include <btBulletDynamicsCommon.h>

static bool init;
static btBroadphaseInterface * broadphase;
static btDefaultCollisionConfiguration * config;
static btCollisionDispatcher * dispatcher;
static btSequentialImpulseConstraintSolver * solver;
static btDiscreteDynamicsWorld * bworld;

static std::vector<entityState_t *> active_states;

static btCollisionShape * testPlane;
static btDefaultMotionState * testPlaneState;
static btRigidBody::btRigidBodyConstructionInfo * testPlaneRigidInfo;
static btRigidBody * testPlaneRigidBody;

static btCollisionShape * testObject;
static btDefaultMotionState * testObjectState;
static btRigidBody::btRigidBodyConstructionInfo * testObjectRigidInfo;
static btRigidBody * testObjectRigidBody;

static btScalar mass {1};
static btVector3 inertia {0, 0, 0};

static void B_ConfigureStateAdd(entityState_t * ent) {
	ent->eFlags |= EF_BULLET_PHYS;
}

static void B_ConfigureStateRem(entityState_t * ent) {
	ent->eFlags &= ~EF_BULLET_PHYS;
}

void BG_InitializeSimulation() {
	if (init) return;

	broadphase = new btDbvtBroadphase;
	config = new btDefaultCollisionConfiguration;
	dispatcher = new btCollisionDispatcher (config);
	solver = new btSequentialImpulseConstraintSolver;
	bworld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);

	bworld->setGravity(btVector3(0, -10, 0));

	testPlane = new btStaticPlaneShape (btVector3(0, 1, 0), 1);
	testPlaneState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
	testPlaneRigidInfo = new btRigidBody::btRigidBodyConstructionInfo(0, testPlaneState, testPlane, btVector3(0, 0, 0));
	testPlaneRigidBody = new btRigidBody(*testPlaneRigidInfo);
	bworld->addRigidBody(testPlaneRigidBody);

	testObject = new btSphereShape (1);
	testObjectState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));
	testObject->calculateLocalInertia(mass, inertia);
	testObjectRigidInfo = new btRigidBody::btRigidBodyConstructionInfo(mass, testObjectState, testObject, inertia);
	testObjectRigidBody = new btRigidBody(*testObjectRigidInfo);
	bworld->addRigidBody(testObjectRigidBody);

	init = true;
}

void BG_ShutdownSimulation() {
	if (!init) return;

	for (entityState_t * es : active_states) {
		B_ConfigureStateRem(es);
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

	btTransform trans;
	testObjectRigidBody->getMotionState()->getWorldTransform(trans);

	Com_Printf("Test Obj Height: %f\n", (float)trans.getOrigin().getY());
	//std::cout << "sphere height: " << trans.getOrigin().getY() << std::endl;
}

void BG_RegisterBPhysEntity(entityState_t * ent) {
	if (!init) return;

	auto iterator = active_states.begin();
	for (size_t i = 0; i < active_states.size(); i++, iterator++) {
		if (*iterator == ent) {
			return;
		}
	}

	B_ConfigureStateAdd(ent);
	active_states.push_back(ent);
}

void BG_UnregisterBPhysEntity(entityState_t * ent) {
	if (!init) return;

	auto iterator = active_states.begin();
	for (size_t i = 0; i < active_states.size(); i++, iterator++) {
		if (*iterator == ent) {
			B_ConfigureStateRem(ent);
			active_states.erase(iterator);
			break;
		}
	}
}
