#include <QtWidgets>
#include "SettingsOperator.h"

#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariant>

MagnifyingGlass::SettingsOperator::SettingsOperator(const std::string &settingsPath)
	: settingsPath(settingsPath)
{
}

void MagnifyingGlass::SettingsOperator::readSettings()
{
	QString document;
	QFile file(settingsPath.c_str());
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		throw std::runtime_error("Failed to read application settings");
	}
	document = file.readAll();
	file.close();

	QJsonDocument jsonDocument = QJsonDocument::fromJson(document.toUtf8());

	if (jsonDocument.isNull())
	{
		throw std::runtime_error("Failed to create the JSON document");
	}

	if (!jsonDocument.isObject())
	{
		throw std::runtime_error("JSON is not an object");
	}

	QJsonObject jsonObject = jsonDocument.object();

	if (jsonObject.isEmpty())
	{
		throw std::runtime_error("The settings file is empty!");
	}

	QVariantMap variantMap = jsonObject.toVariantMap();

	widthPercent = variantMap["widthPercent"].toInt();
	heightPercent = variantMap["heightPercent"].toInt();
	zoomLevel = variantMap["zoomLevel"].toFloat();
	screenShotDirectory = variantMap["screenShotDirectory"].toString();
	hueRotation = variantMap["hueRotation"].toInt();
	sharpenAmount = variantMap["sharpenAmount"].toFloat();
	inputMethods = variantMap["inputMethods"].toMap();
}

void MagnifyingGlass::SettingsOperator::writeSettings()
{
}
