#pragma once

#include "task.h"
#include "data/library.h"
#include "video/webcam.h"
#include "video/frameprocess.h"

class RecorderTask : public Task
{
  public:
    RecorderTask(LibraryPtr library, bool simplify);

    virtual void keyPress(Key key);

    virtual void update(float delta);

    virtual void render(Screen &screen);

  private:
    LibraryPtr library;

    bool simplify;
    bool isSampling;
    std::pair<Point, Size> currentSamplePoint;
    Webcam webcam;
    FramePtr lastFrame;
    FrameProcessor processor;
};

SHARED_PTR(RecorderTask)
