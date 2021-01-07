#ifndef PhysxBase_h
#define PhysxBase_h

#include <ctype.h>
#include "PxConfig.h"
#include "PxPhysicsAPI.h"

using namespace physx;

const float gTimestep = 1.0f/60.0f;

PxDefaultAllocator gAllocator;
PxDefaultErrorCallback gErrorCallback;
PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics	= NULL;
PxDefaultCpuDispatcher*	gDispatcher = NULL;
PxScene* gScene	= NULL;
PxMaterial*	gMaterial = NULL;
PxCooking* gCooking = NULL;

void initPhysics()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	PxTolerancesScale tolerance = PxTolerancesScale();
	tolerance.length = 1;
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, tolerance, true);

	PxSceneDesc sceneDesc(tolerance);
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	PxU32 numWorkers = 2;
	gDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher	= gDispatcher;
	sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxCookingParams params(tolerance);
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams = PxMeshPreprocessingFlags(
		PxMeshPreprocessingFlag::eWELD_VERTICES
	);
	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, params);
}

void stepPhysics()
{
	gScene->simulate(gTimestep);
	gScene->fetchResults(true); // this blocks main thread until simulation results are available
}

#endif