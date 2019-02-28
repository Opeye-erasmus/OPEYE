#include <QtWidgets>
#include "EyeTrackerInput.h"
#include <QWidget>
#include <QPoint>

namespace
{
const unsigned int DAMPING_SPAN = 10;
}

MagnifyingGlass::EyeTrackerInput::EyeTrackerInput()
	: lastPoint(1.0f, 1.0f)
{
	running.store(false);
}

void MagnifyingGlass::EyeTrackerInput::stop()
{
	running.store(false);
	thread.join();
}
MagnifyingGlass::EyeTrackerInput::~EyeTrackerInput()
{
	stop();
}

QPoint MagnifyingGlass::EyeTrackerInput::getInputPosition()
{
	auto point = this->update();
	QScreen *screen = QGuiApplication::primaryScreen();
	return QPoint(point.x() * screen->size().width(), point.y() * screen->size().height());
}

MagnifyingGlass::AdditionalInput MagnifyingGlass::EyeTrackerInput::getAdditionalSignal(QKeyEvent *event) const
{
	if (event->key() == Qt::Key_Escape)
	{
		return MagnifyingGlass::AdditionalInput::Close;
	}
	else if (event->key() == Qt::Key_P)
	{
		return MagnifyingGlass::AdditionalInput::ScreenShot;
	}
	return MagnifyingGlass::AdditionalInput::None;
}

void MagnifyingGlass::EyeTrackerInput::assertRunning()
{
	if (running.load())
		return;
	running.store(true);
	thread = std::thread([this]() {
		TrackerPtr tracker;
		Webcam webcam;
		FramePtr lastFrame;
		while (webcam.hasFrame() && running.load())
		{
			lastFrame = webcam.nextFrame();

			if (!tracker)
			{
				int levels = 4;
				while (!tracker)
				{
					try
					{
						tracker = TrackerPtr(new Tracker(levels, "network", "net_0"));
					}
					catch (std::exception ex)
					{
						std::cout << "Failed to load tracker at level: " << levels << ". Falling back." << std::endl;
						levels--;
						if (levels == 0)
							throw std::runtime_error("No network found!");
					}
				};
				std::cout << "Loaded eye tracker network at level: " << levels << "." << std::endl;
			}
			Rect r = tracker->track(lastFrame);

			lastPoints.push_back(Point({r.min.x + r.dimension.w / 2, r.min.y + r.dimension.h / 2}));
			while (lastPoints.size() > DAMPING_SPAN)
			{
				lastPoints.pop_front();
			}

			double cnt = 1.0f;
			double tot = 1.0f;
			double x = 0.0f;
			double y = 0.0f;
			for (const Point &p : lastPoints)
			{
				x += p.x * cnt;
				y += p.y * cnt;
				tot += cnt;
				cnt++;
			}

			x /= tot;
			y /= tot;
			{
				LOCK(updateLock);
				lastPoint = QPointF(x, y);
			}
		}
	});
}

QPointF MagnifyingGlass::EyeTrackerInput::update()
{
	assertRunning();

	LOCK(updateLock);
	return lastPoint;
}