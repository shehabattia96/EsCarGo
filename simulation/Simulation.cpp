#include <iostream>
#include "Simulation.h"
#include "PhysxIntegration.h"
#include "Sidebar.cpp"

bool isInitPhysics = false;

std::set<int> pressedKeys;
	
void Simulation::draw()
{ // this is called every frame per second per window
	onDraw(mCam, false);

	// Draw the settings widget
	settingsSidebarParameters->draw();

	// interact with physics engine
	if (isInitPhysics) {
		stepVehiclePhysics();
		stepPhysics();
	}

	// draw scene
	drawSimulationObjects(this->simulationObjectsMap);
}

void Simulation::setup()
{
	std::cout << "Setting up GUI" << std::endl;
	mCameraUi.setCamera( &mCam );
	/***** GUI Settings *****/
	// setFullScreen(true);
	setFrameRate(60);
	gl::enableVerticalSync();

	// enable Z-buffering to make shapes smoother around the edges https://libcinder.org/docs/guides/opengl/part2.html
	gl::enableDepthRead();
	gl::enableDepthWrite();

	// Setup our default camera
	mCam.lookAt(vec3(40, 50, 70),vec3(0, 0, -25)); // lookAt(vec3 eyePoint, vec3 target). eyePoint controls rotation, target controls translation.

	/***** Create Settings Widget *****/

	// Create an SettingsSidebarStruct to keep track of the selected object
	this->settingsSidebar = SettingsSidebarStruct();
	// Generate Settings widget
	updateSettingsSidebarParameters(true);

	
	/***** Load models *****/
	initPhysics();
	initVehiclePhysics();
	initWorld();
	updateSettingsSidebarParameters(true);
	isInitPhysics = true;
}

void Simulation::update()
{
	if (mPrintFps && getElapsedFrames() % 60 == 0)
		console() << getAverageFps() << endl;

		
	if (pressedKeys.size() > 0) {
		for (int pressedKey : pressedKeys) {
			switch (pressedKey)
				{
					case KeyEvent::KEY_LEFT:
						startTurnHardLeftMode(); break;
					case KeyEvent::KEY_RIGHT:
						startTurnHardRightMode(); break;
					case KeyEvent::KEY_UP:
						startAccelerateForwardsMode();
						break;
					case KeyEvent::KEY_DOWN:
						startAccelerateReverseMode();
						break;
					default:
						break;
				}
		}
	} else {
		releaseAllControls();
	}
}

void Simulation::mouseDown(MouseEvent event)
{
	mCameraUi.mouseDown( event );
}
void Simulation::mouseDrag(MouseEvent event)
{
	mCameraUi.mouseDrag( event );
}

void Simulation::resize()
{
	onResizeWindow(this->mCam); // helps keep the drawn objects' scale when the GUI window is resized.
}

void Simulation::createSimulationObject(string objectId, PxRigidActor* actor, geom::SourceMods shape, gl::GlslProgRef shader, ci::ColorA* color)
{
	SimulationObject simulationObject(shape, shader);
	if (color != NULL) simulationObject.color = *color;
	simulationObject.preDrawFunction = [this, actor, objectId] () {
			PxTransform actorTransform = actor->getGlobalPose();
			PxMat44 pose(actorTransform);
			
			std::cout<<"preDraw: "<<actorTransform.p.x<<" "<<actorTransform.p.y<<" "<<actorTransform.p.z<<" "<<std::endl;
			// std::cout<<"preDraw: "<<std::endl;
			// {
			// 	PxVec4 col = pose.column0;
			// 	std::cout<< col.x <<" "<< col.y <<" "<< col.z <<" "<< col.w <<" "<<std::endl;
			// 	col = pose.column1;
			// 	std::cout<< col.x <<" "<< col.y <<" "<< col.z <<" "<< col.w <<" "<<std::endl;
			// 	col = pose.column2;
			// 	std::cout<< col.x <<" "<< col.y <<" "<< col.z <<" "<< col.w <<" "<<std::endl;
			// 	col = pose.column3;
			// 	std::cout<< col.x <<" "<< col.y <<" "<< col.z <<" "<< col.w <<" "<<std::endl;
			// }
			mat4 mat(pose.column0.x, pose.column0.y, pose.column0.z, pose.column0.w, 
								pose.column1.x, pose.column1.y, pose.column1.z, pose.column1.w,
								pose.column2.x, pose.column2.y, pose.column2.z, pose.column2.w,
								pose.column3.x, pose.column3.y, pose.column3.z, pose.column3.w);

			SimulationObject* simulationObject = &(this->simulationObjectsMap[objectId]);
			simulationObject->worldPose = mat;
	};
	// add the SimulationObject to a map so we can retreive it by name
	this->simulationObjectsMap[objectId] = simulationObject;
}

void Simulation::keyDown(KeyEvent event) {
	pressedKeys.insert(event.getCode());
}

void Simulation::keyUp(KeyEvent event) {
	pressedKeys.erase(event.getCode());
}

// enum collision_flags {
// 	collision_ground = 1 << 0,
// 	collision_wheel = 1 << 1,
// 	collision_chassis = 1 << 2,
// 	collision_obstruction = 1 << 3
// };
void Simulation::initWorld() {

	// Create ground plane
	{
		PxPlane groundPlaneShape = PxPlane(0,1,0,0);
		PxRigidStatic* groundPlaneActor = PxCreatePlane(*gPhysics, groundPlaneShape, *gMaterial);

		gScene->addActor(*groundPlaneActor);

		geom::SourceMods geomGroundPlaneShape = geom::Cube() >> geom::Scale(glm::vec3(0.1, 200, 200));
		ci::ColorA whiteColor = ci::ColorA( CM_RGB, 1, 1, 1, 1 );

		createSimulationObject("ground", groundPlaneActor, geomGroundPlaneShape, NULL, &whiteColor);
	}

	// Create go-kart
	{
		// PxTransform startingTransform(PxVec3(0,0,0));
		// PxVec3 chassisDim(1.981f, 0.635f, 1.321f); // in m, 78" long, 25" tall, 52" wide
		// PxShape* chassisShape = gPhysics->createShape(PxBoxGeometry(chassisDim), *gMaterial);
		// PxRigidDynamic* chassisActor = gPhysics->createRigidDynamic(startingTransform);
		// chassisActor->attachShape(*chassisShape);
		// PxRigidBodyExt::updateMassAndInertia(*chassisActor, 10.0f);
		// gScene->addActor(*chassisActor);

		VehicleDesc vehicleDesc = initVehicleDesc();
		geom::SourceMods geomChassisShape = geom::Cube() >> geom::Scale(glm::vec3(vehicleDesc.chassisDims.x, vehicleDesc.chassisDims.y, vehicleDesc.chassisDims.z));
		physx::PxRigidDynamic* actor = gVehicle4W->getRigidDynamicActor();
		// createSimulationObject("chassis", actor, geomChassisShape, NULL, NULL);
		// gVehicle4W->getRigidDynamicActor()->setLinearVelocity(PxVec3(-0.1,-0.3f,-50.f));
		
    const physx::PxU32 numShapes = actor->getNbShapes();
    physx::PxShape** shapes = (physx::PxShape**)gAllocator.allocate(sizeof(physx::PxShape*)*numShapes, nullptr, __FILE__, __LINE__);
    actor->getShapes(shapes, numShapes);
		std::cout<<"numShapes "<<numShapes<<std::endl;
	geom::SourceMods geomCarShape;
    for(physx::PxU32 i = 0; i < numShapes; i++)
    {
        physx::PxShape* shape = shapes[i];
		physx::PxGeometryHolder meshHolder = shape->getGeometry();
		PxTransform localPose = shape->getLocalPose();
		PxConvexMeshGeometry meshShape = meshHolder.convexMesh();
		PxGeometryType::Enum shapeType = shape->getGeometryType();
		std::cout<<"shape "<<i<< " "<<shapeType<<std::endl;
		const PxVec3* verticies = meshShape.convexMesh->getVertices();
		std::cout<<"shape "<<i<< " verticies size "<<sizeof(verticies)<<std::endl;

		TriMesh triMeshShape = TriMesh();
		for(physx::PxU32 v = 0; v<sizeof(verticies); v++) {
			PxVec3 vertex = verticies[v];
			triMeshShape.appendPosition(ci::vec3(vertex.x, vertex.y, vertex.z));
		}
		geomCarShape = geomCarShape & triMeshShape >> geom::Translate(vec3(localPose.p.x, localPose.p.y, localPose.p.z));
	}
		createSimulationObject("car", actor, geomCarShape, NULL, NULL);

	}
	
}

CINDER_APP(Simulation, RendererGl(RendererGl::Options().msaa(16)), prepareSettingsSidebar) // render with anti-aliassing
