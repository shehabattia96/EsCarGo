#include "./Simulation.h"

/**
 *  updateSettingsSidebarParameters creates a Settings Widget if it doesn't exist and re-populates it with parameters.
*/
void Simulation::updateSettingsSidebarParameters(bool updateNamesOfObjectsList)
{
	if (updateNamesOfObjectsList)
	{
		this->settingsSidebar.updateNamesOfObjectsList(&this->simulationObjectsMap);
	}
	settingsSidebarParameters.reset();

	ivec2 paramWindowSize = ivec2(200, 400);
	settingsSidebarParameters = params::InterfaceGl::create(getWindow(), "App parameters", toPixels(paramWindowSize));

	if (this->settingsSidebar.getSelectedObject() != NULL)
	{

		settingsSidebarParameters->addParam(
			"Object Size",
			(function<void(ci::vec3)>)[this](ci::vec3 scale) { this->settingsSidebar.getSelectedObject()->scale = scale; },
			(function<ci::vec3()>)[this]()->ci::vec3 { return this->settingsSidebar.getSelectedObject()->scale; });
		settingsSidebarParameters->addParam(
			"Object Translation",
			(function<void(ci::vec3)>)[this](ci::vec3 translation) { this->settingsSidebar.getSelectedObject()->translation = translation; },
			(function<ci::vec3()>)[this]()->ci::vec3 { return this->settingsSidebar.getSelectedObject()->translation; });
		settingsSidebarParameters->addParam(
			"Object Rotation",
			(function<void(glm::highp_quat)>)[this](glm::highp_quat rotation) { this->settingsSidebar.getSelectedObject()->rotation = rotation; },
			(function<glm::highp_quat()>)[this]()->glm::highp_quat { return this->settingsSidebar.getSelectedObject()->rotation; });
		settingsSidebarParameters->addParam(
			"Object Color",
			(function<void(ci::ColorA)>)[this](ci::ColorA color) { this->settingsSidebar.getSelectedObject()->color = color; },
			(function<ci::ColorA()>)[this]()->ci::ColorA { return this->settingsSidebar.getSelectedObject()->color; });

		settingsSidebarParameters->addSeparator();

		settingsSidebarParameters->addParam("Selected Object", this->settingsSidebar.namesOfObjects, &this->settingsSidebar.selectedObjectIndex)
			.keyDecr("[")
			.keyIncr("]")
			.updateFn([this]() {
				this->settingsSidebar.setSelectedObject(&this->simulationObjectsMap);
			});
	}
	else
	{
		settingsSidebarParameters->addText("Status Text", "label=`Please add an object and click refresh.`");
	}

	settingsSidebarParameters->addSeparator();

	settingsSidebarParameters->addParam("print fps", &mPrintFps).keyIncr("p");

	settingsSidebarParameters->addButton("Refresh", [&]() { Simulation::updateSettingsSidebarParameters(true); });
}