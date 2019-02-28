#pragma once

#include "InputInterface.h"
#include <QWidget>
#include <QPoint>

namespace MagnifyingGlass
{
class StandardInput : virtual public InputInterface
{
	virtual QPoint getInputPosition() override;
	virtual AdditionalInput getAdditionalSignal(QKeyEvent *event) const override;
};
} // namespace MagnifyingGlass