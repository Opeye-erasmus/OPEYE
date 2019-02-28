#pragma once

#include "task.h"
#include "data/library.h"
#include "video/webcam.h"
#include "data/tracker.h"


class EvaluationTask : public Task{
public:
    EvaluationTask(TrackerPtr tracker);

    virtual void keyPress(Key key);

    virtual void update(float delta);

    virtual void render(Screen& screen);

private:
    TrackerPtr tracker;
    Webcam webcam;
    FramePtr lastFrame;

    bool recreate;
    Point rectToLookAt;
    Size dimension;
};

SHARED_PTR(EvaluationTask)
