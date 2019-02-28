#pragma once

#include <QtWidgets/QMainWindow>

#include "ui_MagnifyingGlassWindow.h"
#include "Settings/SettingsOperator.h"
#include "InputInterface/InputInterface.h"
#include "MagnifyingGlassWidget.h"

#include <memory>

class MagnifyingGlassWindow : public QMainWindow
{
	Q_OBJECT

  public:
	MagnifyingGlassWindow(const MagnifyingGlass::SettingsOperator &applicationSettings,
						  std::map<QString, std::shared_ptr<MagnifyingGlass::InputInterface>> interfaceMap,
						  QWidget *parent = Q_NULLPTR);

  private:
	Ui::MagnifyingGlassWindowClass ui;
	MagnifyingGlassWidget magnifyingGlassWidget;

	MagnifyingGlass::SettingsOperator applicationSettings;
	std::map<QString, std::shared_ptr<MagnifyingGlass::InputInterface>> interfaceMap;

	void connectSignals();
	void loadData();

  private slots:
	void openMagnifyingWidget();
};
