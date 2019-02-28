#include <QtWidgets>
#include "StandardInput.h"
#include <QWidget>
#include <QPoint>

QPoint MagnifyingGlass::StandardInput::getInputPosition()
{
	QScreen *screen = QGuiApplication::primaryScreen();
	return QCursor::pos(screen);
}

MagnifyingGlass::AdditionalInput MagnifyingGlass::StandardInput::getAdditionalSignal(QKeyEvent *event) const
{
	if (event->key() == Qt::Key_Escape)
	{
		return MagnifyingGlass::AdditionalInput::Close;
	}
	if (event->key() == Qt::Key_P)
	{
		return MagnifyingGlass::AdditionalInput::ScreenShot;
	}
	return MagnifyingGlass::AdditionalInput::None;
}