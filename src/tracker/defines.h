#pragma once

#include <dlib/dnn.h>
#include <memory>
#include <thread>
#include <mutex>
#include <json.hpp>

using json = nlohmann::json;

#define SHARED_PTR(name) typedef std::shared_ptr<name> name##Ptr;
#define LOCK(name) std::lock_guard<std::mutex> lock(name);

/**
 * Simple mouse button identifiers
 */
enum Button
{
    BUTTON_NONE,
    BUTTON_RIGHT,
    BUTTON_LEFT
};

/**
 * A few key identifiers
 */
enum Key
{
    KEY_NONE,
    KEY_SPACE,
    KEY_ESCAPE,
    KEY_RETURN,
    KEY_1,
    KEY_2,
    KEY_3
};

/**
 * Common 2D point sturct
 */
struct Point
{
    float x;
    float y;
};

/**
 * Common 2D size sturct
 */
struct Size
{
    float w;
    float h;
};

/**
 * Common 2D rectangle struct
 */
struct Rect
{
    Point min;
    Size dimension;
};

/**
 * The dimension of the processor (i.e. input image size )
 * 
 * Larger image size yields better results but is very computational intensive. Try to stick with powers of two.
 */
static const unsigned int PROCESSOR_SIZE = 256;

/**
 * The structure of the neural network.
 * 
 * The input is sampled with 2 convolutions of 5x5 with at a stride of 2x2. Then fed to a fully connected 128 nodes layer.
 */
template <typename SUBNET>
using quarterLocator =
    dlib::relu<dlib::fc<128,
                        dlib::con<2, 5, 5, 2, 2,
                                  SUBNET>>>;
;
