#pragma once

#include "defines.h"
#include "video/frame.h"


#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>
#include <dlib/image_io.h>

class FrameProcessor {
public:
    FrameProcessor(unsigned int w=512, unsigned int h=512);

    FramePtr operator()(FramePtr frame);

    static void normalize(cv::Mat& image);
    static void computeNormals(const cv::Mat& input, cv::Mat& output);

private:
    bool getEyeMarkers(const cv::Mat &frame, std::vector<std::vector<dlib::point>> &eyeMarkers);
    cv::Mat extract(const cv::Mat& image, const dlib::rectangle& eyeRegion);
    dlib::rectangle getEyeBounds(const std::vector<dlib::point>& markers) const;



    unsigned int width;
    unsigned int height;

    dlib::frontal_face_detector detector;
    dlib::shape_predictor poseModel;

    unsigned int sourceEyeMargin;
    std::mutex mutex;
};
