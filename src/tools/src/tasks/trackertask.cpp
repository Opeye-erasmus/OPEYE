#include <iostream>
#include "trackertask.h"

TrackerTask::TrackerTask(TrackerPtr tracker_)
    : tracker(tracker_) {
}

void TrackerTask::keyPress(Key key) {

}

void TrackerTask::update(float delta) {

}

void TrackerTask::render(Screen& screen) {
    screen.fill({0.0f, 0.0f}, {1.0f, 1.0f}, 0, 0, 0);

    if (webcam.hasFrame()) {
        lastFrame = webcam.nextFrame();
        Rect r = tracker->track(lastFrame);

        //auto processed = processor(lastFrame);

        screen.fill({0.25f, 0.25f}, {0.25f, 0.25f}, lastFrame, 0.1f);
        //screen.fill({0.0f, 0.0f}, {0.25f, 0.25f}, lastFrame, 0.9f);
        screen.fill(r.min, r.dimension, 255, 0, 0);
    }

}
