#ifndef VehicleConfig_h
#define VehicleConfig_h

#include <ctype.h>
#include "PxPhysicsAPI.h"

#include "cinder/Json.h"

using namespace physx;

typedef struct VehicleConfig {
	std::string name;

	PxVec3 startingPosition;
	PxVec3 startingRotation;

	PxF32 wheelMass;
	PxF32 wheelMOI;
    PxF32 wheelRadius;
	PxF32 wheelWidth;
	PxU32 numWheels;
    PxVec3 wheelCenterActorOffsets;

	PxVec3 chassisCMOffset;
	PxVec3 chassisDims;
    PxF32 chassisMass;
	
	PxF32 motorPeakTorque;
	PxF32 motorMaxOmega; // Omega = RPM / 10.0

	VehicleConfig() {};

} VehicleConfig;

// given a file path to a json file with vehicles' properties, attempts to parse the json file and create a vector of VehicleConfig.
std::vector<VehicleConfig> readVehiclesFromJSON(std::filesystem::path jsonFilePath) {
	std::vector<VehicleConfig> vehicleConfigs;

	ci::JsonTree jsonFileContent( ci::loadFile(jsonFilePath) );
	for( ci::JsonTree::ConstIter vehicleIter = jsonFileContent.begin(); vehicleIter != jsonFileContent.end(); ++vehicleIter ) {
		VehicleConfig vehicleConfig;
        // assign all the variables in the struct to the json object value.
		vehicleConfig.name = vehicleIter -> getChild("name").getValue<std::string>();
        ci::JsonTree startingPosition = vehicleIter -> getChild("startingPosition");
		vehicleConfig.startingPosition = PxVec3(startingPosition.getChild("[0]").getValue<float>(), startingPosition.getChild("[1]").getValue<float>(), startingPosition.getChild("[2]").getValue<float>());
        ci::JsonTree startingRotation = vehicleIter -> getChild("startingRotation");
		vehicleConfig.startingRotation = PxVec3(startingRotation.getChild("[0]").getValue<float>(), startingRotation.getChild("[1]").getValue<float>(), startingRotation.getChild("[2]").getValue<float>());


		vehicleConfig.wheelMass = vehicleIter -> getChild("wheelMass").getValue<float>();
		vehicleConfig.wheelRadius = vehicleIter -> getChild("wheelRadius").getValue<float>();
		vehicleConfig.wheelWidth = vehicleIter -> getChild("wheelWidth").getValue<float>();
		vehicleConfig.numWheels = vehicleIter -> getChild("numWheels").getValue<int>();
        ci::JsonTree wheelCenterActorOffsets = vehicleIter -> getChild("wheelCenterActorOffsets");
		vehicleConfig.wheelCenterActorOffsets = PxVec3(wheelCenterActorOffsets.getChild("[0]").getValue<float>(), wheelCenterActorOffsets.getChild("[1]").getValue<float>(), wheelCenterActorOffsets.getChild("[2]").getValue<float>());

        ci::JsonTree chassisCMOffset = vehicleIter -> getChild("chassisCMOffset");
		vehicleConfig.chassisCMOffset = PxVec3(chassisCMOffset.getChild("[0]").getValue<float>(), chassisCMOffset.getChild("[1]").getValue<float>(), chassisCMOffset.getChild("[2]").getValue<float>());
        ci::JsonTree chassisDims = vehicleIter -> getChild("chassisDims");
		vehicleConfig.chassisDims = PxVec3(chassisDims.getChild("[0]").getValue<float>(), chassisDims.getChild("[1]").getValue<float>(), chassisDims.getChild("[2]").getValue<float>());
		
		vehicleConfig.chassisMass = vehicleIter -> getChild("chassisMass").getValue<float>();

		vehicleConfig.motorPeakTorque = vehicleIter -> getChild("motorPeakTorque").getValue<float>();
		vehicleConfig.motorMaxOmega = vehicleIter -> getChild("motorMaxOmega").getValue<float>();


		vehicleConfigs.push_back(vehicleConfig);
	}

	return vehicleConfigs;
}

#endif