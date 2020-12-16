#include "./Simulation.h"

/**
 *  updateSettingsSideBarParameters creates a Settings Widget if it doesn't exist and re-populates it with parameters.
*/
void Simulation::updateSettingsSideBarParameters(bool updateNamesOfObjectsList)
{
	if (updateNamesOfObjectsList)
	{
		this->settingsSideBar.updateNamesOfObjectsList(&this->simulationObjectsMap);
	}
	settingsSideBarParameters.reset();

	ivec2 paramWindowSize = ivec2(200, 400);
	settingsSideBarParameters = params::InterfaceGl::create(getWindow(), "App parameters", toPixels(paramWindowSize));

	if (this->settingsSideBar.getSelectedObject() != NULL)
	{

		settingsSideBarParameters->addParam(
			"Object Size",
			(function<void(ci::vec3)>)[this](ci::vec3 scale) { this->settingsSideBar.getSelectedObject()->scale = scale; },
			(function<ci::vec3()>)[this]()->ci::vec3 { return this->settingsSideBar.getSelectedObject()->scale; });
		settingsSideBarParameters->addParam(
			"Object Translation",
			(function<void(ci::vec3)>)[this](ci::vec3 translation) { this->settingsSideBar.getSelectedObject()->translation = translation; },
			(function<ci::vec3()>)[this]()->ci::vec3 { return this->settingsSideBar.getSelectedObject()->translation; });
		settingsSideBarParameters->addParam(
			"Object Rotation",
			(function<void(glm::highp_quat)>)[this](glm::highp_quat rotation) { this->settingsSideBar.getSelectedObject()->rotation = rotation; },
			(function<glm::highp_quat()>)[this]()->glm::highp_quat { return this->settingsSideBar.getSelectedObject()->rotation; });
		settingsSideBarParameters->addParam(
			"Object Color",
			(function<void(ci::ColorA)>)[this](ci::ColorA color) { this->settingsSideBar.getSelectedObject()->color = color; },
			(function<ci::ColorA()>)[this]()->ci::ColorA { return this->settingsSideBar.getSelectedObject()->color; });

		settingsSideBarParameters->addSeparator();

		settingsSideBarParameters->addParam("Selected Object", this->settingsSideBar.namesOfObjects, &this->settingsSideBar.selectedObjectIndex)
			.keyDecr("[")
			.keyIncr("]")
			.updateFn([this]() {
				this->settingsSideBar.setSelectedObject(&this->simulationObjectsMap);
			});
	}
	else
	{
		settingsSideBarParameters->addText("Status Text", "label=`Please add an object and click refresh.`");
	}

	settingsSideBarParameters->addSeparator();

	settingsSideBarParameters->addParam("print fps", &mPrintFps).keyIncr("p");

	settingsSideBarParameters->addButton("Refresh", [&]() { Simulation::updateSettingsSideBarParameters(true); });
}