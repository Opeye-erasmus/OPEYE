#pragma once
#include <dlib/opencv.h>
#include "video/frame.h"

typedef std::shared_ptr<struct Sample> SamplePtr;
typedef std::shared_ptr<struct RegionNode> RegionNodePtr;
typedef std::shared_ptr<std::thread> ThreadPtr;

/**
 * A sample is a simple picture with the position the subject gazed at during recording.
 */
struct Sample
{
    Point point;
    dlib::matrix<dlib::rgb_pixel> frame;
    unsigned int id;
    bool onDisk;
    bool inMemory;
};

/**
 * A node that holds a set of samples with respect to a screen region
 */
struct RegionNode
{
    RegionNodePtr children[4];
    unsigned int depth;
    Point min;
    std::vector<SamplePtr> samples;
};
