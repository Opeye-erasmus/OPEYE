#pragma once

#include "task.h"
#include "data/library.h"
#include "video/webcam.h"
#include "data/calibration.h"
#include "data/tracker.h"


class MagnifyTask : public Task{
public:
    MagnifyTask(TrackerPtr tracker);

    virtual void keyPress(Key key);

    virtual void update(float delta);

    virtual void render(Screen& screen);

    virtual void mouseClick(Button button, float x, float y);

private:
    TrackerPtr tracker;
    Webcam webcam;
    FramePtr lastFrame;

    std::list<Point> lastPoints;
    std::list<Point> lastResulting;

    Calibration calibration;
    Point lookAt;
    bool useLookAt;
};

SHARED_PTR(MagnifyTask)
