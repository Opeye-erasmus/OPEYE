#include "webcam.h"


Webcam::Webcam()
    : webcam(0)
{
    webcam.set(CV_CAP_PROP_FRAME_WIDTH,1960);
    webcam.set(CV_CAP_PROP_FRAME_HEIGHT,1080);
}

bool Webcam::hasFrame() const{
    return webcam.isOpened();
}

FramePtr Webcam::nextFrame() {
    webcam >> frameCache;
    return FramePtr(new Frame(frameCache));
}
