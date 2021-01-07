#include <iostream>
#include "./Simulation.h"
#include "./PhysxIntegration.h"
#include "./PhysxVehicle.h"
#include "./PhysxCinder.h"
#include "./Sidebar.cpp"

bool isInitPhysics = false;

void Simulation::initWorld() {
	initPhysics();
	initMap();
	initVehiclePhysics();
	initVehiclePhysics();
	glMatrixMode(GL_MODELVIEW);
	drawSceneActors();
	isInitPhysics = true;
}

void Simulation::setup()
{
	mCameraUi.setCamera( &mCam );
	/***** GUI Settings *****/
	// setFullScreen(true);
	setFrameRate(60);
	gl::enableVerticalSync();

	// enable Z-buffering to make shapes smoother around the edges https://libcinder.org/docs/guides/opengl/part2.html
	gl::enableDepthRead();
	gl::enableDepthWrite();


	mCam.setPerspective(60.0f, getWindowAspectRatio(), 1.0f, 10000.f);
	// Setup our default camera
	mCam.lookAt(vec3(40, 50, 70),vec3(0, 0, -25)); // lookAt(vec3 eyePoint, vec3 target). eyePoint controls rotation, target controls translation.

	/***** Create Settings Widget *****/

	// Create an SettingsSidebarStruct to keep track of the selected object
	this->settingsSidebar = SettingsSidebarStruct();
	// Generate Settings widget
	updateSettingsSidebarParameters(true);

	
	/***** Load models *****/
	initWorld();
	updateSettingsSidebarParameters(true);
}

// this is called every frame per second per window
void Simulation::draw()
{
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


std::set<int> pressedKeys; // keeps track of pressed arrow keys

size_t currentVehicleBeingControlled = 0;
void cycleVehicleToControl() {
	pressedKeys.clear(); releaseAllControls(&vehiclesInputData[currentVehicleBeingControlled],static_cast<PxVehicleDrive4W*>(vehiclesWheels[currentVehicleBeingControlled])); stepVehiclePhysics(); // stop the last car
	currentVehicleBeingControlled = currentVehicleBeingControlled + 1 >= vehiclesWheels.size() ? 0 : ++currentVehicleBeingControlled;
}

void Simulation::keyDown(KeyEvent event) {
	pressedKeys.insert(event.getCode());
	
	if (event.getCode() == KeyEvent::KEY_PERIOD)
		cycleVehicleToControl();
}

void Simulation::keyUp(KeyEvent event) {
	pressedKeys.erase(event.getCode());
}

void Simulation::update()
{
	if (mPrintFps && getElapsedFrames() % 60 == 0)
		console() << getAverageFps() << endl;

		
	releaseAllControls(&vehiclesInputData[currentVehicleBeingControlled], static_cast<PxVehicleDrive4W*>(vehiclesWheels[currentVehicleBeingControlled]));
	if (pressedKeys.size() > 0) {
		for (int pressedKey : pressedKeys) {
			switch (pressedKey)
				{
					case KeyEvent::KEY_a:
					case KeyEvent::KEY_LEFT:
						startTurnHardRightMode(&vehiclesInputData[currentVehicleBeingControlled]);
						break;
					case KeyEvent::KEY_d:
					case KeyEvent::KEY_RIGHT:
						startTurnHardLeftMode(&vehiclesInputData[currentVehicleBeingControlled]); 
						break;
					case KeyEvent::KEY_w:
					case KeyEvent::KEY_UP:
						startAccelerateForwardsMode(&vehiclesInputData[currentVehicleBeingControlled]);
						break;
					case KeyEvent::KEY_s:
					case KeyEvent::KEY_DOWN:
						startAccelerateReverseMode(&vehiclesInputData[currentVehicleBeingControlled],static_cast<PxVehicleDrive4W*>(vehiclesWheels[currentVehicleBeingControlled]));
						break;
					case KeyEvent::KEY_LSHIFT:
					case KeyEvent::KEY_SPACE:
						startBrakeMode(&vehiclesInputData[currentVehicleBeingControlled]);
						break;
					default:
						break;
				}
		}
	}
}

void Simulation::mouseDown(MouseEvent event)
{
	mCameraUi.mouseDown( event ); // uses Cinder Camera UI to give factory camera controls to the scene.
}
void Simulation::mouseDrag(MouseEvent event)
{
	mCameraUi.mouseDrag( event ); // uses Cinder Camera UI to give factory camera controls to the scene.
}

void Simulation::resize()
{
	onResizeWindow(this->mCam); // helps keep the drawn objects' scale when the GUI window is resized.
}

/**
 * createSimulationObject 
 * 1. Initializes a SimulationObject instance with a Cinder shape and shader, and a unique objectId. 
 * 2. The objectId can be used later to manipulate the Cinder source to update the shape if needed.
 * 3. A preDrawFunction is created that retreives Global Pose from the PhysX actor to transform the gl matrix.
 * @param - objectId - A unique identifier for the object
 * @param - actor - the PhysX actor used to retrieve global pose.
 * @param - shape - A Cinder source such as geom::Cube() >> geom::Scale(glm::vec3(2))
 * @param - shader - A glsl program reference (see Cinder documentation)
 * @param - color - A cinder RGBA color (in case shader is NULL, this will affect the color of the source).
 * */
void Simulation::createSimulationObject(string objectId, PxRigidActor* actor, geom::SourceMods shape, gl::GlslProgRef shader, ci::ColorA* color)
{
	SimulationObject simulationObject(shape, shader);
	if (color != NULL) simulationObject.color = *color;
	if (actor != NULL)
		simulationObject.preDrawFunction = [this, actor, objectId] () {
				PxTransform actorTransform = actor->getGlobalPose();
				PxMat44 pose(actorTransform);
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

CINDER_APP(Simulation, RendererGl(RendererGl::Options().msaa(16)), prepareSettingsSidebar) // render with anti-aliassing
