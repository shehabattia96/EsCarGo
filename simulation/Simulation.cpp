#include "Simulation.h"
#include "PhysxBase.h"
#include "Sidebar.cpp"

bool isInitPhysics = false;

void Simulation::draw()
{ // this is called every frame per second per window
	onDraw(mCam, false);

	// Draw the settings widget
	settingsSideBarParameters->draw();

	// interact with physics engine
	if (isInitPhysics) stepPhysics();

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

	// Setup our default camera
	mCam.lookAt(vec3(40, 50, 70),vec3(0, 0, -25)); // lookAt(vec3 eyePoint, vec3 target). eyePoint controls rotation, target controls translation.

	/***** Create Settings Widget *****/

	// Create an SettingsSideBarStruct to keep track of the selected object
	this->settingsSideBar = SettingsSideBarStruct();
	// Generate Settings widget
	updateSettingsSideBarParameters(true);

	
	/***** Load models *****/
	initPhysics();
	isInitPhysics = true;
}

void Simulation::update()
{
	if (mPrintFps && getElapsedFrames() % 60 == 0)
		console() << getAverageFps() << endl;
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
	simulationObject.preDrawFunction = [this, actor,objectId] () {
			PxMat44 pose(actor->getGlobalPose());
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

CINDER_APP(Simulation, RendererGl(RendererGl::Options().msaa(16)), prepareSettingsSideBar) // render with anti-aliassing
