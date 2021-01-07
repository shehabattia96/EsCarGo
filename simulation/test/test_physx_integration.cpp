#include <iostream>
#include <catch2/catch.hpp>
#include "cinder/app/App.h"

#include "../PhysxVehicle.h"

using namespace std;

TEST_CASE("VehicleConfig")
{
	SECTION("Reading JSON file")
	{
        std::filesystem::path filePath = ci::app::getAppPath() / std::filesystem::path ("resources/sampleJson.json");
        std::vector<VehicleConfig> output = readVehiclesFromJSON(filePath);
        
		REQUIRE( output.size() == 1 );
		REQUIRE( output[0].name == "kart1" );
		REQUIRE( output[0].startingPosition.x == 1 );
		REQUIRE( output[0].startingRotation.x == 1.0 );

		REQUIRE( output[0].wheelMass == 1.0 );
		REQUIRE( output[0].wheelRadius == 1.0 );
		REQUIRE( output[0].wheelWidth == 1.0 );
		REQUIRE( output[0].numWheels == 4 );
		REQUIRE( output[0].wheelCenterActorOffsets.x == 1 );

		REQUIRE( output[0].chassisMass == 1.0 );
		REQUIRE( output[0].chassisDims.x == 1.0 );
		

		REQUIRE( output[0].motorPeakTorque == 500.0);
		REQUIRE( output[0].motorMaxOmega == 600.0 );
	}
}
