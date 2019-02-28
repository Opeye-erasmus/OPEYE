#include "frame.h"


Frame::Frame(unsigned int w, unsigned int h)
    : frame(w, h, CV_8UC3, cv::Scalar(0, 0, 0)){
}

Frame::Frame(const cv::Mat& frame_)
    : frame(frame_) {

}

const cv::Mat& Frame::get() const {
    return frame;
}

cv::Mat& Frame::get(){
    return frame;
}
