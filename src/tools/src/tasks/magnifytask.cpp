#include <iostream>
#include "magnifytask.h"

namespace {
    const unsigned int DAMPING_SPAN = 10;
    float lastX = 0.0f;
    float lastY = 0.0f;
}

MagnifyTask::MagnifyTask(TrackerPtr tracker_)
    : tracker(tracker_)
    , useLookAt(false)
{

}

void MagnifyTask::mouseClick(Button button, float x, float y) {
    useLookAt = false;
    if (button == BUTTON_RIGHT) {
        lookAt.x=x;
        lookAt.y=y;
        useLookAt = true;
    } else {
        calibration.addCalibrationPoint(x,y, lastX, lastY);
    }
//    calibration.addCalibrationPoint(x,y, lastX, lastY);
}

void MagnifyTask::keyPress(Key key) {

}

void MagnifyTask::update(float delta) {

}

void MagnifyTask::render(Screen& screen) {
    screen.fill({0.0f, 0.0f}, {1.0f, 1.0f}, 64,64,64);

    if (webcam.hasFrame()) {
        lastFrame = webcam.nextFrame();
        Rect r = tracker->track(lastFrame);

        lastPoints.push_back(Point({r.min.x + r.dimension.w/2, r.min.y + r.dimension.h/2}));
        while(lastPoints.size()>DAMPING_SPAN) {
            lastPoints.pop_front();
        }

        float cnt = 0.0f;
        float x = 0.0f;
        float y = 0.0f;
        for (const Point& p : lastPoints) {
            x += p.x;
            y += p.y;
            cnt++;
        }

        x /= cnt;
        y /= cnt;

        Point result = calibration.correct(Point({x, y}));
        x = result.x;
        y = result.y;

        const float dim = 0.05f;
        if (x < dim){
            x = dim;
        }
        if (y < dim){
            y = dim;
        }
        if (x > 1.0f- dim){
            x = 1.0f- dim;
        }
        if (y > 1.0f-dim){
            y = 1.0f-dim;
        }
        float dimWebX = 0.25f;
        float dimWebY = dimWebX;
        float appliedX = 0.5*x+0.5*lastX;
        float appliedY = 0.5*y+0.5*lastY;
        lastResulting.push_back(Point({appliedX, appliedY}));



        while(lastResulting.size()>20) {
            lastResulting.pop_front();
        }
        float d = 1.0f/20.0f;
        for (const Point& p : lastResulting){
            screen.fillCircle({p.x, p.y}, 0.1f*d, 200,150*d,100*d);
            d += 1.0f/20.0f;
        }

        if (useLookAt) {
            screen.fillCircle({lookAt.x, lookAt.y}, 0.03f, 100,200,150);
        }

        cv::flip(lastFrame->frame, lastFrame->frame, 1 );

        screen.fill({0.5f-dimWebX/2.0f, 0.5f-dimWebY/2.0f}, {dimWebX, dimWebY}, lastFrame, 0.7f);
        lastX = x;
        lastY = y;

    }
}
