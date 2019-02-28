#include <QtWidgets>
#include <iostream>
#include "MagnifyingGlassWindow.h"

MagnifyingGlassWindow::MagnifyingGlassWindow(const MagnifyingGlass::SettingsOperator &applicationSettings,
											 std::map<QString, std::shared_ptr<MagnifyingGlass::InputInterface>> interfaceMap_,
											 QWidget *parent)
	: applicationSettings(applicationSettings),
	  interfaceMap(interfaceMap_),
	  QMainWindow(parent)
{
	ui.setupUi(this);

	loadData();
	connectSignals();
}

void MagnifyingGlassWindow::connectSignals()
{
	connect(ui.startZoomingButton, &QPushButton::clicked, this, &MagnifyingGlassWindow::openMagnifyingWidget);
}

void MagnifyingGlassWindow::loadData()
{
	ui.widthSpinBox->setValue(applicationSettings.widthPercent);
	ui.heightSpinBox->setValue(applicationSettings.heightPercent);
	ui.screenShotDirectoryLineEdit->setText(applicationSettings.screenShotDirectory);
	ui.hueRotationSpinBox->setValue(applicationSettings.hueRotation);
	ui.sharpenAmountSpinBox->setValue(applicationSettings.sharpenAmount);

	QStringList inputMethods;
	for (auto it = interfaceMap.begin();
		 it != interfaceMap.end();
		 ++it)
	{
		inputMethods.push_back(it->first);
	}
	ui.inputMethodComboBox->addItems(inputMethods);
}

void MagnifyingGlassWindow::openMagnifyingWidget()
{
	QScreen *screen = QGuiApplication::primaryScreen();
	magnifyingGlassWidget.setFixedWidth(screen->geometry().width() * ui.widthSpinBox->value() / 100);
	magnifyingGlassWidget.setFixedHeight(screen->geometry().height() * ui.heightSpinBox->value() / 100);
	magnifyingGlassWidget.setZoomLevel(ui.zoomLevelSpinBox->minimum() + ui.zoomLevelSpinBox->maximum() - ui.zoomLevelSpinBox->value());
	magnifyingGlassWidget.setScreenShotDirectory(ui.screenShotDirectoryLineEdit->text().toStdString());
	magnifyingGlassWidget.setHueRotation(ui.hueRotationSpinBox->value());
	magnifyingGlassWidget.setSharpenAmount(ui.sharpenAmountSpinBox->value());

	std::shared_ptr<MagnifyingGlass::InputInterface> inputMethod = interfaceMap[ui.inputMethodComboBox->currentText()];
	magnifyingGlassWidget.setInputInterface(inputMethod);
	magnifyingGlassWidget.show();
	magnifyingGlassWidget.reinitialize();
}