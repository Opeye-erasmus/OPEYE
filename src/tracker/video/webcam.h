#pragma once

#include "defines.h"
#include "frame.h"

class Webcam {
public:
    Webcam();

    virtual bool hasFrame() const;

    virtual FramePtr nextFrame();
private:
    cv::VideoCapture webcam;
    cv::Mat frameCache;
};
