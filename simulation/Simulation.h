#ifndef Simulation_h
#define Simulation_h
#include "cinder/CameraUi.h"
#include <ctype.h>
#include "PxConfig.h"
#include "PxPhysicsAPI.h"

#include "../externals/Cinder-Simulation-App/src/CinderAppHelper.h"

using namespace std;
using namespace physx;

class Simulation : public App {
  public:
	void setup() override;
	void keyDown(KeyEvent event) override;
	void keyUp(KeyEvent event) override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void update() override;
	void draw() override;
	void resize();
	void updateSettingsSidebarParameters(bool updateNamesOfObjectsList);
	void initWorld();
  private:
  	CameraPersp	mCam;
	CameraUi mCameraUi;
	std::map<std::string, SimulationObject::type> simulationObjectsMap;
	SettingsSidebarStruct::type settingsSidebar;
	params::InterfaceGlRef settingsSidebarParameters;
	bool mPrintFps;
	void drawSceneActors();
	void createSimulationObject(string objectId, PxRigidActor* actor, geom::SourceMods shape, gl::GlslProgRef shader, ci::ColorA* color);
};

#endif