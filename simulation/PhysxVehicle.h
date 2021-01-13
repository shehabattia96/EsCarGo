#ifndef SimulationVehicle_h
#define SimulationVehicle_h

#include "vehicle/PxVehicleUtil.h" // PhysX api
#include "cinder/ObjLoader.h"

#include "./PhysXIntegration.h"
#include "./SimulationVehicleConfig.h"

// Located at ./include/snippetvehiclecommon and linked via cmake:
#include "SnippetVehicleSceneQuery.h"
#include "SnippetVehicleFilterShader.h"
#include "SnippetVehicleTireFriction.h"
#include "SnippetVehicleCreate.h"


using namespace physx;
using namespace snippetvehicle;

// Vehicle SDK

// based on physx\snippets\snippetvehicle4w\SnippetVehicle4W.cpp
VehicleSceneQueryData* gVehicleSceneQueryData = NULL;
PxBatchQuery* gBatchQuery = NULL;

PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

PxRigidStatic* gGroundPlane = NULL;

bool gIsVehicleInAir = true;
bool gMimicKeyInputs = true;

// based on physx\snippets\snippetvehicle4w\SnippetVehicle4W.cpp
PxF32 gSteerVsForwardSpeedData[2*8] =
{
	0.0f,		0.75f,
	5.0f,		0.75f,
	30.0f,		0.125f,
	120.0f,		0.1f,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32
};
// based on physx\snippets\snippetvehicle4w\SnippetVehicle4W.cpp
PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData,4);

// based on physx\snippets\snippetvehicle4w\SnippetVehicle4W.cpp
PxVehicleKeySmoothingData gKeySmoothingData=
{
	{
		6.0f,	//rise rate eANALOG_INPUT_ACCEL
		6.0f,	//rise rate eANALOG_INPUT_BRAKE		
		6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f,	//fall rate eANALOG_INPUT_ACCEL
		10.0f,	//fall rate eANALOG_INPUT_BRAKE		
		10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
		5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

// based on physx\snippets\snippetvehicle4w\SnippetVehicle4W.cpp
PxVehiclePadSmoothingData gPadSmoothingData=
{
	{
		6.0f,	//rise rate eANALOG_INPUT_ACCEL
		6.0f,	//rise rate eANALOG_INPUT_BRAKE		
		6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f,	//fall rate eANALOG_INPUT_ACCEL
		10.0f,	//fall rate eANALOG_INPUT_BRAKE		
		10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
		5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
	}
};


// based on physx\snippets\snippetvehicle4w\SnippetVehicle4W.cpp
VehicleDesc initVehicleDesc()
{
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const PxF32 chassisMass = 500.0f;
	const PxVec3 chassisDims(2.5f,.5f,5.0f);
	const PxVec3 chassisMOI
		((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass/12.0f,
		 (chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass/12.0f,
		 (chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass/12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y*0.5f + 0.65f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 20.0f;
	const PxF32 wheelRadius = 0.5f;
	const PxF32 wheelWidth = 0.4f;
	const PxF32 wheelMOI = 0.5f*wheelMass*wheelRadius*wheelRadius;
	const PxU32 nbWheels = 4;

	VehicleDesc vehicleDesc;

	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisCMOffset = chassisCMOffset;
	vehicleDesc.chassisMaterial = gMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

	vehicleDesc.wheelMass = wheelMass;
	vehicleDesc.wheelRadius = wheelRadius;
	vehicleDesc.wheelWidth = wheelWidth;
	vehicleDesc.wheelMOI = wheelMOI;
	vehicleDesc.numWheels = nbWheels;
	vehicleDesc.wheelMaterial = gMaterial;
	vehicleDesc.chassisSimFilterData = PxFilterData(COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

	return vehicleDesc;
}



PxRigidDynamic* initVehicleActor(const VehicleDesc& vehicle4WDesc, PxConvexMesh* wheelMesh, PxConvexMesh* chassisConvexMesh, PxPhysics* physics)
{
	const PxU32 numWheels = vehicle4WDesc.numWheels;

	const PxFilterData& chassisSimFilterData = vehicle4WDesc.chassisSimFilterData;
	const PxFilterData& wheelSimFilterData = vehicle4WDesc.wheelSimFilterData;
	//Construct a physx actor with shapes for the chassis and wheels.
	//Set the rigid body mass, moment of inertia, and center of mass offset.
	PxRigidDynamic* veh4WActor = NULL;
	{
		//Assume all wheels are identical for simplicity.
		PxConvexMesh* wheelConvexMeshes[PX_MAX_NB_WHEELS];
		PxMaterial* wheelMaterials[PX_MAX_NB_WHEELS];

		//Set the meshes and materials for the driven wheels.
		for(PxU32 i = PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= PxVehicleDrive4WWheelOrder::eREAR_RIGHT; i++)
		{
			wheelConvexMeshes[i] = wheelMesh;
			wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
		}
		//Set the meshes and materials for the non-driven wheels
		for(PxU32 i = PxVehicleDrive4WWheelOrder::eREAR_RIGHT + 1; i < numWheels; i++)
		{
			wheelConvexMeshes[i] = wheelMesh;
			wheelMaterials[i] = vehicle4WDesc.wheelMaterial;
		}

		PxConvexMesh* chassisConvexMeshes[1] = {chassisConvexMesh};
		PxMaterial* chassisMaterials[1] = {vehicle4WDesc.chassisMaterial};

		//Rigid body data.
		PxVehicleChassisData rigidBodyData;
		rigidBodyData.mMOI = vehicle4WDesc.chassisMOI;
		rigidBodyData.mMass = vehicle4WDesc.chassisMass;
		rigidBodyData.mCMOffset = vehicle4WDesc.chassisCMOffset;

		veh4WActor = createVehicleActor
			(rigidBodyData,
			wheelMaterials, wheelConvexMeshes, numWheels, wheelSimFilterData,
			chassisMaterials, chassisConvexMeshes, 1, chassisSimFilterData,
			*physics);
	}

	return veh4WActor;
}

std::vector<PxVehicleWheels*> vehiclesWheels;
std::vector<PxVehicleDrive4WRawInputData> vehiclesInputData;

void initMap()
{
	//Create a plane to drive on.
	PxFilterData groundPlaneSimFilterData(COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	gGroundPlane = createDrivablePlane(groundPlaneSimFilterData, gMaterial, gPhysics);
	gScene->addActor(*gGroundPlane);
}

void initVehiclePhysics()
{
	PxInitVehicleSDK(*gPhysics);
	PxVehicleSetBasisVectors(PxVec3(0,1,0), PxVec3(0,0,1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eACCELERATION);

	//Create the batched scene queries for the suspension raycasts.
	physx::PxU32 numVehicles = static_cast<physx::PxU32>(vehiclesWheels.size()) + 1;
	gVehicleSceneQueryData = VehicleSceneQueryData::allocate(numVehicles, PX_MAX_NB_WHEELS, 1, numVehicles, WheelSceneQueryPreFilterBlocking, NULL, gAllocator);
	gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, gScene);

	//Create the friction table for each combination of tire and surface type.
	gFrictionPairs = createFrictionPairs(gMaterial);

	//Create a vehicle that will drive on the plane.
	VehicleDesc vehicleDesc = initVehicleDesc();
	
	//Wheel mesh
	PxConvexMesh* wheelMesh = snippetvehicle::createWheelMesh(vehicleDesc.wheelWidth, vehicleDesc.wheelRadius, *gPhysics, *gCooking);
	//Chassis mesh
	PxConvexMesh* chassisConvexMesh;
	ObjLoader loader( loadFile( "./resources/models/temp_obj/chassis.obj" ) );
	TriMesh mesh(loader);
	size_t numVerticies = mesh.getNumVertices();
	if (numVerticies > 0) {
		//Scale chassis mesh so that it fits chassisDimensions
		AxisAlignedBox boundingBox = mesh.calcBoundingBox();
		vec3 maxDimensions = boundingBox.getMax();
		float scalingFactor = (vehicleDesc.chassisDims.z / 2.0f) / maxDimensions.z;
		mesh = mesh >> geom::Scale(scalingFactor);
		// copy verticies from TriMesh to PxVec3 vector:
		std::vector<PxVec3> physxShapeVerticies;
		const ci::vec3* positions = mesh.getPositions<3>();
		for (size_t i = 0; i < numVerticies; ++i) {
			ci::vec3 position = positions[i];
			PxVec3 vertex = PxVec3(position.x, position.y, position.z);
			physxShapeVerticies.push_back(vertex);
		}
		chassisConvexMesh = snippetvehicle::createConvexMesh(&physxShapeVerticies[0], static_cast<PxU32>(numVerticies), *gPhysics, *gCooking);
	} else {
		chassisConvexMesh = createChassisMesh(vehicleDesc.chassisDims, *gPhysics, *gCooking);
	}
	PxRigidDynamic* veh4WActor = initVehicleActor(vehicleDesc, wheelMesh, chassisConvexMesh, gPhysics);
	PxVehicleDrive4W* vehicle4W = snippetvehicle::createVehicle4W(vehicleDesc, veh4WActor, gPhysics, gCooking);
	vehiclesWheels.push_back(vehicle4W);

	const PxTransform startTransform(PxVec3(vehiclesWheels.size() * vehicleDesc.chassisDims.x * 2, (vehicleDesc.chassisDims.y*0.5f + vehicleDesc.wheelRadius + 5.0f), 0), PxQuat(PxIdentity));
	vehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	gScene->addActor(*vehicle4W->getRigidDynamicActor());

	//Set the vehicle to rest in first gear.
	vehicle4W->setToRestState();
	vehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	vehicle4W->mDriveDynData.setUseAutoGears(true);

	PxVehicleDrive4WRawInputData vehicleInputData;
	vehiclesInputData.push_back(vehicleInputData);
}



// based on physx\snippets\snippetvehicle4w\SnippetVehicle4W.cpp
enum DriveMode
{
	eDRIVE_MODE_ACCEL_FORWARDS=0,
	eDRIVE_MODE_ACCEL_REVERSE,
	eDRIVE_MODE_HARD_TURN_LEFT,
	eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
	eDRIVE_MODE_HARD_TURN_RIGHT,
	eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_NONE
};

void startAccelerateForwardsMode(PxVehicleDrive4WRawInputData* input)
{
	if(gMimicKeyInputs)
	{
		input->setDigitalAccel(true);
	}
	else
	{
		input->setAnalogAccel(1.0f);
	}
}

void startAccelerateReverseMode(PxVehicleDrive4WRawInputData* input, PxVehicleDrive4W* vehicle)
{
	vehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);

	if(gMimicKeyInputs)
	{
		input->setDigitalAccel(true);
	}
	else
	{
		input->setAnalogAccel(1.0f);
	}
}

void startBrakeMode(PxVehicleDrive4WRawInputData* input)
{
	if(gMimicKeyInputs)
	{
		input->setDigitalBrake(true);
	}
	else
	{
		input->setAnalogBrake(1.0f);
	}
}

void startTurnHardLeftMode(PxVehicleDrive4WRawInputData* input)
{
	if(gMimicKeyInputs)
	{
		input->setDigitalSteerLeft(true);
	}
	else
	{
		input->setAnalogSteer(-1.f);
	}
}

void startTurnHardRightMode(PxVehicleDrive4WRawInputData* input)
{
	if(gMimicKeyInputs)
	{
		input->setDigitalSteerRight(true);
	}
	else
	{
		input->setAnalogSteer(1.f);
	}
}


void releaseAllControls(PxVehicleDrive4WRawInputData* input, PxVehicleDrive4W* vehicle)
{
	if (vehicle->mDriveDynData.getCurrentGear() == PxVehicleGearsData::eREVERSE)
		vehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	if(gMimicKeyInputs)
	{
		input->setDigitalAccel(false);
		input->setDigitalSteerLeft(false);
		input->setDigitalSteerRight(false);
		input->setDigitalBrake(false);
		input->setDigitalHandbrake(false);
	}
	else
	{
		input->setAnalogAccel(0.0f);
		input->setAnalogSteer(0.0f);
		input->setAnalogBrake(0.0f);
		input->setAnalogHandbrake(0.0f);
	}
}

void stepVehiclePhysics()
{
	physx::PxU32 numVehicles = static_cast<physx::PxU32>(vehiclesWheels.size());
	if (numVehicles == 0) return;

	//Update the control inputs for the vehicle.
	for (int i = 0; i < vehiclesWheels.size(); i++) {
		PxVehicleDrive4W* vehicle = static_cast<PxVehicleDrive4W*>(vehiclesWheels[i]);
		PxVehicleDrive4WRawInputData inputData = vehiclesInputData[i];
		if(gMimicKeyInputs)
		{
			PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable, inputData, gTimestep, gIsVehicleInAir, *vehicle);
		}
		else
		{
			PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, inputData, gTimestep, gIsVehicleInAir, *vehicle);
		}
	}

	//Raycasts.
	PxVehicleWheels** vehicleWheels = &vehiclesWheels[0];
	PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	const PxU32 raycastResultsSize = gVehicleSceneQueryData->getQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(gBatchQuery, numVehicles, vehicleWheels, raycastResultsSize, raycastResults);

	//Vehicle update.
	const PxVec3 grav = gScene->getGravity();
	PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
	PxVehicleWheelQueryResult vehicleQueryResults[100];
	for (int i = 0; i < vehiclesWheels.size(); i++) {
		vehicleQueryResults[i] = {wheelQueryResults, vehiclesWheels[i]->mWheelsSimData.getNbWheels()};
	}
	PxVehicleUpdates(gTimestep, grav, *gFrictionPairs, numVehicles, vehicleWheels, vehicleQueryResults);

	//Work out if the vehicle is in the air.
	// gIsVehicleInAir = gVehicle4W->getRigidDynamicActor()->isSleeping() ? false : PxVehicleIsInAir(vehicleQueryResults[0]);
}

#endif