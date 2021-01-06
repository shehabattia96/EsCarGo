#include <iostream>
#include "Simulation.h"
#include "PhysxIntegration.h"
#include "PhysxCinder.h"
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
	mCameraUi.setCamera( &mCam );
	/***** GUI Settings *****/
	// setFullScreen(true);
	setFrameRate(60);
	gl::enableVerticalSync();

	// enable Z-buffering to make shapes smoother around the edges https://libcinder.org/docs/guides/opengl/part2.html
	gl::enableDepthRead();
	gl::enableDepthWrite();

	
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	mCam.setPerspective(60.0f, getWindowAspectRatio(), 1.0f, 10000.f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Setup our default camera
	// mCam.lookAt(vec3(40, 50, 70),vec3(0, 0, -25)); // lookAt(vec3 eyePoint, vec3 target). eyePoint controls rotation, target controls translation.
	mCam.lookAt(vec3(10.0f, 10.0f, 10.0f),vec3(-0.6f, -0.2f, -0.7f)); // lookAt(vec3 eyePoint, vec3 target). eyePoint controls rotation, target controls translation.

	glColor4f(0.4f, 0.4f, 0.4f, 1.0f);

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

		
	releaseAllControls();
	if (pressedKeys.size() > 0) {
		for (int pressedKey : pressedKeys) {
			switch (pressedKey)
				{
					case KeyEvent::KEY_LEFT:
						startTurnHardRightMode();
						break;
					case KeyEvent::KEY_RIGHT:
						startTurnHardLeftMode(); 
						break;
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
			// std::cout<<"preDraw: "<<actorTransform.p.x<<" "<<actorTransform.p.y<<" "<<actorTransform.p.z<<" "<<std::endl;
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
void Simulation::initWorld() {	
	glMatrixMode(GL_MODELVIEW);
	drawSceneActors();
}

CINDER_APP(Simulation, RendererGl(RendererGl::Options().msaa(16)), prepareSettingsSidebar) // render with anti-aliassing
