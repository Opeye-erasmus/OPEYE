#include <iostream>
#include "recordertask.h"

RecorderTask::RecorderTask(LibraryPtr library_, bool simplify_)
    : library(library_), isSampling(false), processor(PROCESSOR_SIZE, PROCESSOR_SIZE), simplify(simplify_)
{
}

void RecorderTask::keyPress(Key key)
{
    if (key == KEY_SPACE && isSampling && lastFrame)
    {
        float x = currentSamplePoint.first.x + currentSamplePoint.second.w / 2.0f;
        float y = currentSamplePoint.first.y + currentSamplePoint.second.h / 2.0f;
        library->addSample({x, y}, lastFrame);
        isSampling = false;
    }
}

void RecorderTask::update(float delta)
{
    if (!isSampling)
    {
        isSampling = true;
        currentSamplePoint = library->getSparseRegionRandomPoint();
        if (simplify)
        {
            double scale = 0.1;
            currentSamplePoint.first.x -= 0.5 * scale;
            currentSamplePoint.first.y -= 0.5 * scale;
            currentSamplePoint.second.w += 1.0 * scale;
            currentSamplePoint.second.h += 1.0 * scale;
        }
    }
}

void RecorderTask::render(Screen &screen)
{
    screen.fill({0.0f, 0.0f}, {1.0f, 1.0f}, 0, 0, 0);

    if (webcam.hasFrame())
    {
        lastFrame = webcam.nextFrame();

        if (!simplify)
            screen.fill({0.25f, 0.25f}, {0.25f, 0.25f}, FramePtr(new Frame(lastFrame->get().clone())), 0.1f);

        FramePtr processed = processor(FramePtr(new Frame(lastFrame->get().clone())));
        if (processed)
        {
            if (!simplify)
                screen.fill({0.0f, 0.0f}, {0.25f, 0.25f}, processed, 0.2f);
        }
        else
        {
            lastFrame = nullptr;
        }
    }

    if (isSampling)
    {
        if (!simplify)
        {
            screen.fill(currentSamplePoint.first, currentSamplePoint.second, 127, 255, 0);
        }
        else
        {
            screen.fill(currentSamplePoint.first, currentSamplePoint.second, 255, 255, 255);
        }
    }
}
