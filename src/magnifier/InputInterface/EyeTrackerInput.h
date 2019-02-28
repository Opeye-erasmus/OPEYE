#pragma once

#include "InputInterface.h"
#include <QWidget>
#include <QPoint>
#include <thread>
#include <mutex>
#define LOCK(name) std::lock_guard<std::mutex> lock(name);

#include <data/library.h>
#include <video/webcam.h>
#include <data/calibration.h>
#include <data/tracker.h>

namespace MagnifyingGlass
{
class EyeTrackerInput : virtual public InputInterface
{
  public:
	EyeTrackerInput();
	~EyeTrackerInput();
	virtual QPoint getInputPosition() override;

	virtual AdditionalInput getAdditionalSignal(QKeyEvent *event) const override;
	virtual void stop();

  private:
	QPointF update();
	void assertRunning();

	std::list<Point> lastPoints;

	std::thread thread;
	std::atomic<bool> running;
	std::mutex updateLock;
	QPointF lastPoint;
};
} // namespace MagnifyingGlass