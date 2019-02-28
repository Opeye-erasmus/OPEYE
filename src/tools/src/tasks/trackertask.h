#pragma once

#include "task.h"
#include "data/library.h"
#include "video/webcam.h"
#include "data/tracker.h"


class TrackerTask : public Task{
public:
    TrackerTask(TrackerPtr tracker);

    virtual void keyPress(Key key);

    virtual void update(float delta);

    virtual void render(Screen& screen);

private:
    TrackerPtr tracker;

    Webcam webcam;
    FramePtr lastFrame;

    FrameProcessor processor;
};

SHARED_PTR(TrackerTask)
