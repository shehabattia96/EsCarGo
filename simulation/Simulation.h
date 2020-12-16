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
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void update() override;
	void draw() override;
	void resize();
	void updateSettingsSideBarParameters(bool updateNamesOfObjectsList);
	void createSimulationObject(string objectId, PxRigidActor* actor, geom::SourceMods shape, gl::GlslProgRef shader, ci::ColorA* color);
	void initPhysics();
  private:
  	CameraPersp	mCam;
	CameraUi mCameraUi;
	std::map<std::string, SimulationObject::type> simulationObjectsMap;
	SettingsSideBarStruct::type settingsSideBar;
	params::InterfaceGlRef settingsSideBarParameters;
	bool mPrintFps;
};
#endif