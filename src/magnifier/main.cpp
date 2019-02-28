#include <QtWidgets>

#include "MagnifyingGlassWindow.h"
#include <QtWidgets/QApplication>

#include "Settings/SettingsOperator.h"
#include "InputInterface/InputInterface.h"
#include "InputInterface/StandardInput.h"
#include "InputInterface/EyeTrackerInput.h"

#include <memory>
#include <map>

std::map<std::string, std::shared_ptr<MagnifyingGlass::InputInterface>> inputInterfacesMap = {
	{"StandardInput", std::make_shared<MagnifyingGlass::StandardInput>()},
	{"EyeTrackerInput", std::make_shared<MagnifyingGlass::EyeTrackerInput>()}};

std::map<QString,
		 std::shared_ptr<MagnifyingGlass::InputInterface>>
createInterfaceMap(MagnifyingGlass::SettingsOperator applicationSettings)
{

	std::map<QString, std::shared_ptr<MagnifyingGlass::InputInterface>> result;

	for (auto it = applicationSettings.inputMethods.begin();
		 it != applicationSettings.inputMethods.end();
		 ++it)
	{
		if (inputInterfacesMap.find(it.value().toString().toStdString()) != inputInterfacesMap.end())
		{
			result[it.key()] = inputInterfacesMap[it.value().toString().toStdString()];
		}
	}
	return result;
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	MagnifyingGlass::SettingsOperator applicationSettings(
		"./settings.json");
	applicationSettings.readSettings();

	MagnifyingGlassWindow w(applicationSettings,
							createInterfaceMap(applicationSettings));
	w.show();
	return a.exec();
}
