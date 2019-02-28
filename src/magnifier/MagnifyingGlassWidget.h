#pragma once

#include <QWidget>
#include "ui_MagnifyingGlassWidget.h"
#include <memory>
#include "InputInterface/InputInterface.h"

#include <QWidget>
#include <QOpenGLWidget>
#include <QPixmap>
#include <QScreen>
#include <QTimer>
#include <GL/glu.h>
#include <GL/gl.h>

#include <fstream>

class MagnifyingGlassWidget : public QOpenGLWidget
{
	Q_OBJECT

  protected:
	void initializeGL();
	void paintGL();

  public:
	MagnifyingGlassWidget(QWidget *parent = Q_NULLPTR);
	~MagnifyingGlassWidget();

	inline void setInputInterface(std::shared_ptr<MagnifyingGlass::InputInterface> inputInterface)
	{
		this->inputInterface = inputInterface;
	}

	void moveEvent(QMoveEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;

	inline void setZoomLevel(const float zoomLevel)
	{
		this->zoomLevel = zoomLevel;
	}

	inline void setScreenShotDirectory(const std::string &screenShotDirectory)
	{
		this->screenShotDirectory = screenShotDirectory;
	}

	inline void setHueRotation(const int hueRotation)
	{
		this->hueRotation = hueRotation;
	}

	inline void setSharpenAmount(const float sharpenAmount)
	{
		this->sharpenAmount = sharpenAmount;
	}

	void reinitialize();

  private:
	Ui::MagnifyingGlassWidget ui;
	std::shared_ptr<MagnifyingGlass::InputInterface> inputInterface;
	GLuint vertexShader;
	GLuint passthroughVertexShader;
	GLuint fragmentShader;
	GLuint colorConvertShader;
	GLuint glProgram;
	GLuint glColorConvertProgram;
	GLuint screenTexture;
	GLuint magnifiedTexture;
	QPixmap pixmap;
	QTimer timer, timerPosition;
	QPoint lastPosition;
	QImage lastImage;
	bool render;
	float zoomLevel;
	int hueRotation;
	float sharpenAmount;
	std::string screenShotDirectory;
	int makeScreenshot;

	QPixmap screenShoot();
	QPixmap screenShootInner();
	void updatePosition();

	int loadShader(std::string &shaderPath, std::string *shaderSource);
};
