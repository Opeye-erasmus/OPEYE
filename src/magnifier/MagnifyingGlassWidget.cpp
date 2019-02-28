#include "MagnifyingGlassWidget.h"

#include <QtWidgets>
#include <QOpenGLShader>
#include <QDesktopWidget>
#include <fstream>
#include <sstream>
#include <iostream>

MagnifyingGlassWidget::MagnifyingGlassWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
	this->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
	this->setAttribute(Qt::WA_NoSystemBackground, true);
	this->setAttribute(Qt::WA_TranslucentBackground, true);
	this->setWindowModality(Qt::ApplicationModal);

	setMouseTracking(true);
	ui.setupUi(this);
	this->setWindowTitle("Click this window, then space to update.");
}

MagnifyingGlassWidget::~MagnifyingGlassWidget()
{
}

void MagnifyingGlassWidget::reinitialize()
{
	timerPosition.setInterval(60);
	connect(&timerPosition, &QTimer::timeout, this, &MagnifyingGlassWidget::updatePosition);
	lastPosition = QPoint(0, 0);
	timerPosition.start();
	render = true;

	this->screenShoot();
}

void MagnifyingGlassWidget::initializeGL()
{
	QOpenGLFunctions *openGLFunctions = QOpenGLContext::currentContext()->functions();

	fragmentShader = openGLFunctions->glCreateShader(GL_FRAGMENT_SHADER);
	std::string shaderPath = "./shaders/magnifier.frag";
	std::string shaderSource;
	int length = loadShader(shaderPath, &shaderSource);
	const GLchar *fragmentSource = (const GLchar *)shaderSource.c_str();
	openGLFunctions->glShaderSource(fragmentShader, 1, &fragmentSource, &length);
	openGLFunctions->glCompileShader(fragmentShader);

	vertexShader = openGLFunctions->glCreateShader(GL_VERTEX_SHADER);
	shaderPath = "./shaders/magnifier.vert";
	shaderSource = "";
	length = loadShader(shaderPath, &shaderSource);
	const GLchar *vertexSource = (const GLchar *)shaderSource.c_str();
	openGLFunctions->glShaderSource(vertexShader, 1, &vertexSource, &length);
	openGLFunctions->glCompileShader(vertexShader);

	colorConvertShader = openGLFunctions->glCreateShader(GL_FRAGMENT_SHADER);
	shaderPath = "./shaders/colorConvert.frag";
	shaderSource = "";
	length = loadShader(shaderPath, &shaderSource);
	const GLchar *colorConvertFragmentSource = (const GLchar *)shaderSource.c_str();
	openGLFunctions->glShaderSource(colorConvertShader, 1, &colorConvertFragmentSource, &length);
	openGLFunctions->glCompileShader(colorConvertShader);

	passthroughVertexShader = openGLFunctions->glCreateShader(GL_VERTEX_SHADER);
	shaderPath = "./shaders/passthrough.vert";
	shaderSource = "";
	length = loadShader(shaderPath, &shaderSource);
	const GLchar *passthroughVertexSource = (const GLchar *)shaderSource.c_str();
	openGLFunctions->glShaderSource(passthroughVertexShader, 1, &passthroughVertexSource, &length);
	openGLFunctions->glCompileShader(passthroughVertexShader);

	glProgram = openGLFunctions->glCreateProgram();
	openGLFunctions->glAttachShader(glProgram, vertexShader);
	openGLFunctions->glAttachShader(glProgram, fragmentShader);
	openGLFunctions->glLinkProgram(glProgram);

	glColorConvertProgram = openGLFunctions->glCreateProgram();
	openGLFunctions->glAttachShader(glColorConvertProgram, passthroughVertexShader);
	openGLFunctions->glAttachShader(glColorConvertProgram, colorConvertShader);
	openGLFunctions->glLinkProgram(glColorConvertProgram);

	openGLFunctions->glEnable(GL_TEXTURE_2D);
	openGLFunctions->glGenTextures(1, &screenTexture);
	openGLFunctions->glGenTextures(1, &magnifiedTexture);
	openGLFunctions->glDisable(GL_TEXTURE_2D);

	openGLFunctions->glClearColor(0, 0, 1, 1);
	openGLFunctions->glEnable(GL_DEPTH_TEST);
	openGLFunctions->glEnable(GL_LIGHT0);
	openGLFunctions->glEnable(GL_LIGHTING);
	//openGLFunctions->glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	//openGLFunctions->glEnable(GL_COLOR_MATERIAL);

	screenShoot();
}

void MagnifyingGlassWidget::paintGL()
{
	updatePosition();
	GLuint activeProgram = glProgram;
	QOpenGLFunctions *openGLFunctions = QOpenGLContext::currentContext()->functions();
	openGLFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();

	QImage image = render ? pixmap.toImage() : lastImage;
	image = image.convertToFormat(QImage::Format_RGBA8888);
	openGLFunctions->glActiveTexture(GL_TEXTURE0 + 0);
	openGLFunctions->glBindTexture(GL_TEXTURE_2D, screenTexture);
	openGLFunctions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(),
								  0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

	GLint resolutionLocation = openGLFunctions->glGetUniformLocation(activeProgram, "resolution");
	GLint samplerLocation = openGLFunctions->glGetUniformLocation(activeProgram, "screenSampler");
	GLint windowSizeLocation = openGLFunctions->glGetUniformLocation(activeProgram, "windowSize");
	GLint startPositionLocation = openGLFunctions->glGetUniformLocation(activeProgram, "startPosition");
	GLint zoomLevelLocation = openGLFunctions->glGetUniformLocation(activeProgram, "zoomLevel");
	openGLFunctions->glUseProgram(activeProgram);

	openGLFunctions->glUniform1i(samplerLocation, 0);
	openGLFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	openGLFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	openGLFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	openGLFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	openGLFunctions->glUniform2f(resolutionLocation, static_cast<float>(image.width()), static_cast<float>(image.height()));
	openGLFunctions->glUniform2f(windowSizeLocation, static_cast<float>(this->width()), static_cast<float>(this->height()));
	openGLFunctions->glUniform2f(startPositionLocation, static_cast<float>(this->pos().x()), static_cast<float>(this->pos().y()));
	openGLFunctions->glUniform1f(zoomLevelLocation, zoomLevel);
	openGLFunctions->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	if (render)
	{
		lastImage = pixmap.toImage();
	}
	QImage magnifiedImage = this->grabFramebuffer();
	magnifiedImage = magnifiedImage.convertToFormat(QImage::Format_RGBA8888);
	openGLFunctions->glActiveTexture(0);
	openGLFunctions->glUseProgram(0);

	openGLFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	openGLFunctions->glActiveTexture(GL_TEXTURE0);
	openGLFunctions->glBindTexture(GL_TEXTURE_2D, magnifiedTexture);

	openGLFunctions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, magnifiedImage.width(), magnifiedImage.height(),
								  0, GL_RGBA, GL_UNSIGNED_BYTE, magnifiedImage.bits());

	GLint magnifiedScreen = openGLFunctions->glGetUniformLocation(glColorConvertProgram, "magnifiedScreen");
	GLint hueRotationLocation = openGLFunctions->glGetUniformLocation(glColorConvertProgram, "hueRotation");
	GLint windowResolutionLocation = openGLFunctions->glGetUniformLocation(glColorConvertProgram, "resolution");
	GLint sharpenAmountLocation = openGLFunctions->glGetUniformLocation(glColorConvertProgram, "sharpenAmount");
	openGLFunctions->glUseProgram(glColorConvertProgram);
	openGLFunctions->glUniform1i(magnifiedScreen, 0);
	openGLFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	openGLFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	openGLFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	openGLFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	openGLFunctions->glUniform1f(hueRotationLocation, hueRotation);
	openGLFunctions->glUniform2f(windowResolutionLocation, static_cast<float>(this->width()), static_cast<float>(this->height()));
	openGLFunctions->glUniform1f(sharpenAmountLocation, sharpenAmount);
	openGLFunctions->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	openGLFunctions->glActiveTexture(0);
	openGLFunctions->glUseProgram(0);
	render = false;
}

void MagnifyingGlassWidget::moveEvent(QMoveEvent *event)
{
	update();
}

void MagnifyingGlassWidget::updatePosition()
{
	QPoint mousePosition = inputInterface->getInputPosition();
	if (std::abs(mousePosition.x() - lastPosition.x()) > 0 || std::abs(mousePosition.y() - lastPosition.y()) > 0)
	{
		mousePosition = mousePosition * 0.05f + lastPosition * 0.95f;
		this->move(mousePosition - QPoint(this->width() / 2, this->height() / 2));
		render = true;
		this->update();
	}
	else
	{
		render = false;
	}
	lastPosition = mousePosition;

	if (makeScreenshot > 0)
	{
		if (--makeScreenshot == 0)
		{
			screenShootInner();
			this->setWindowOpacity(1.0);
			setUpdatesEnabled(true);
		}
	}
}

void MagnifyingGlassWidget::mouseMoveEvent(QMouseEvent *event)
{
	updatePosition();
}

void MagnifyingGlassWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Space:
	{
		this->screenShoot();
	}
	break;
	}

	switch (inputInterface->getAdditionalSignal(event))
	{
	case MagnifyingGlass::AdditionalInput::Close:
		this->close();
		timer.stop();
		timerPosition.stop();
		inputInterface->stop();
		break;
	case MagnifyingGlass::AdditionalInput::ScreenShot:
		QPixmap toSave = screenShootInner();
		std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
		std::string path = screenShotDirectory + "/" + std::to_string(currentTime.time_since_epoch().count()) + ".png";
		toSave.save(QString(path.c_str()), "PNG");
		break;
	}
}

QPixmap MagnifyingGlassWidget::screenShoot()
{
	setUpdatesEnabled(false);
	this->setWindowOpacity(0.0);
	update();
	makeScreenshot = 5;
	return pixmap;
}

QPixmap MagnifyingGlassWidget::screenShootInner()
{
	QScreen *screen = QGuiApplication::primaryScreen();
	if (const QWindow *window = windowHandle())
	{
		screen = window->screen();
	}
	if (screen)
	{
		render = true;
		pixmap = screen->grabWindow(0);
	}
	return pixmap;
}

int MagnifyingGlassWidget::loadShader(std::string &shaderPath, std::string *shaderSource)
{
	std::ifstream inputFileStream;
	inputFileStream.open(shaderPath.c_str(), std::ios::in);

	std::stringstream buffer;
	buffer << inputFileStream.rdbuf();
	*shaderSource = buffer.str();

	inputFileStream.close();
	return shaderSource->length();
}