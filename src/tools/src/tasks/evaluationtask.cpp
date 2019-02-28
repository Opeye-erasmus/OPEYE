#include <iostream>
#include "evaluationtask.h"

namespace {
    static std::mt19937 generator = std::mt19937();
}

EvaluationTask::EvaluationTask(TrackerPtr tracker_)
    : tracker(tracker_)
    , recreate(true)
{
    dimension.w = 1.0f / (2 << tracker->getDepth());
    dimension.h = dimension.w;
}

void EvaluationTask::keyPress(Key key) {
    if (key == KEY_SPACE) {
        recreate = true;
    }
}

void EvaluationTask::update(float delta) {
    if (recreate) {
        std::uniform_int_distribution<int> distribution(0, 15);
        float x = distribution(::generator);
        float y = distribution(::generator);
        float dim = (2 << tracker->getDepth());
        rectToLookAt.x=x/dim;
        rectToLookAt.y=y/dim;
        //rectToLookAt = {.x=x/dim, .y=y/dim};
        recreate = false;
    }
}

void EvaluationTask::render(Screen& screen) {
    screen.fill({0.0f, 0.0f}, {1.0f, 1.0f}, 0, 0, 0);

    if (webcam.hasFrame()) {
        lastFrame = webcam.nextFrame();
        Rect r = tracker->track(lastFrame);

        screen.fill({0.25f, 0.25f}, {0.25f, 0.25f}, lastFrame, 0.1f);
        screen.fill(r.min, r.dimension, 255, 0, 0);
    }


    screen.fill(rectToLookAt, dimension, 127, 227, 127);

}
