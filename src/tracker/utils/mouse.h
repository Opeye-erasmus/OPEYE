#pragma once

#include "defines.h"
#include "video/screen.h"

class Mouse {
public:
    Mouse(const Screen& screen);

    Button update(Point& Point);

private:
    const Screen& screen;
};
