#pragma once
#include "defines.h"

#include <opencv2/opencv.hpp>

class Frame {
public:
    Frame(unsigned int w, unsigned int h);
    Frame(const cv::Mat& frame);

    const cv::Mat& get() const;
private:
    friend class Screen;
    friend class Library;
    friend class FrameProcessor;
    friend class Tracker;
    friend class RecorderTask;
    friend class MagnifyTask;
    cv::Mat& get();
    cv::Mat frame;
};


SHARED_PTR(Frame)
