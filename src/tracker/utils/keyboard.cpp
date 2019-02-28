#include "keyboard.h"
#include <opencv2/opencv.hpp>

Key Keyboard::update() {
    auto pressed = cv::waitKey(1)&0xff;

    switch(pressed) {
    case 32:
        return KEY_SPACE;
    case 13:
        return KEY_RETURN;
    case 27:
        return KEY_ESCAPE;
    case 49:
        return KEY_1;
    case 50:
        return KEY_2;
    case 51:
        return KEY_3;
    }
    return KEY_NONE;
}
