#ifndef PhysxBase_h
#define PhysxBase_h

#include <ctype.h>
#include "PxConfig.h"
#include "PxPhysicsAPI.h"
using namespace physx;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;
PxFoundation*			gFoundation = NULL;
PxPhysics*				gPhysics	= NULL;
PxDefaultCpuDispatcher*	gDispatcher = NULL;
PxScene*				gScene		= NULL;
PxMaterial*				gMaterial	= NULL;
PxPvd*                  gPvd        = NULL;
PxReal stackZ = 10.0f;


void Simulation::initPhysics()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	PxTolerancesScale tolerance = PxTolerancesScale();
	tolerance.length = 1;
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, tolerance,true);

	PxSceneDesc sceneDesc(tolerance);
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher	= gDispatcher;
	sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxPlane groundPlaneShape = PxPlane(0,1,0,0);
	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, groundPlaneShape, *gMaterial);
	gScene->addActor(*groundPlane);
	geom::SourceMods geomGroundPlaneShape = geom::Cube() >> geom::Scale(glm::vec3(0.1, 200, 200));
	ci::ColorA whiteColor = ci::ColorA( CM_RGB, 1, 1, 1, 1 );
	createSimulationObject("ground", groundPlane, geomGroundPlaneShape, NULL, &whiteColor);
}

void stepPhysics()
{
	gScene->simulate(1.0f/60.0f);
	gScene->fetchResults(true);
}


// TODO:cleanupPhysics
void cleanupPhysics()
{
}



#endif