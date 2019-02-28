#pragma once

#include "defines.h"
#include "frame.h"

class Screen
{
  public:
    Screen(const std::string &name, const unsigned int height);

    void display();

    void fill(const Point &point, const Size &size, const FramePtr topFrame, float blend = 1.0f);

    void fill(const Point &point, const Size &size, unsigned char r, unsigned char g, unsigned char b);
    void fillCircle(const Point &point, const float size, unsigned char r, unsigned char g, unsigned char b);
    void putText(const std::string &text, const Point &position);

    FramePtr getFrame();

    const std::string &getName() const;
    unsigned int getWidth() const;
    unsigned int getHeight() const;

  private:
    std::string name;
    FramePtr frame;
};
