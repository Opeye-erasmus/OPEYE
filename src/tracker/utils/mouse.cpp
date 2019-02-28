#include "mouse.h"
#include <opencv2/opencv.hpp>

namespace {
    Button lastButton = BUTTON_NONE;
    int lastX = 0;
    int lastY = 0;
    void mouseFunc(int event, int x, int y, int flags, void* userdata) {
        lastX = x;
        lastY = y;
        if (event == cv::EVENT_LBUTTONDOWN)
            lastButton = BUTTON_LEFT;
        if (event == cv::EVENT_RBUTTONDOWN)
            lastButton = BUTTON_RIGHT;
    }
}

Mouse::Mouse(const Screen& screen_)
    : screen(screen_)
{
    cv::setMouseCallback(screen.getName(), mouseFunc, NULL);

}

Button Mouse::update(Point& click) {
    if (lastButton != BUTTON_NONE) {
        Button button = lastButton;
        lastButton = BUTTON_NONE;
        click.x = static_cast<float>(lastX) / screen.getWidth();
        click.y = static_cast<float>(lastY) / screen.getHeight();
        std::cout << click.x << ", " << click.y << std::endl;
        return button;
    }
    return BUTTON_NONE;
}
