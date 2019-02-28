#pragma once

#include <QWidget>
#include <QPoint>

namespace MagnifyingGlass
{
enum AdditionalInput
{
	Close,
	ScreenShot,
	None
};

class InputInterface
{
  public:
	virtual QPoint getInputPosition() = 0;
	virtual AdditionalInput getAdditionalSignal(QKeyEvent *event) const = 0;
	//virtual QPointF update() { return QPointF(0, 0); };
	virtual void stop(){};
};
} // namespace MagnifyingGlass
