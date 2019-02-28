#pragma once

#include <string>
#include <QMap>

namespace MagnifyingGlass
{
class SettingsOperator
{
  private:
	const std::string settingsPath;

  public:
	SettingsOperator(const std::string &settingsPath);

	void readSettings();
	void writeSettings();

  public:
	int widthPercent;
	int heightPercent;
	float zoomLevel;
	int hueRotation;
	float sharpenAmount;
	QString screenShotDirectory;
	QMap<QString, QVariant> inputMethods;
};
} // namespace MagnifyingGlass