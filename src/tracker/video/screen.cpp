#include "screen.h"

Screen::Screen(const std::string &name_, const unsigned int height_)
    : name(name_)
{
    cv::namedWindow(name, cv::WINDOW_NORMAL);
    cv::setWindowProperty(name, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    frame = FramePtr(new Frame(height_, static_cast<unsigned int>(static_cast<float>(height_) * (16.0f / 9.0f))));
}

void Screen::display()
{
    cv::imshow(name, frame->get());
    cv::setWindowProperty(name, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
}

void Screen::putText(const std::string &text, const Point &position)
{
    cv::Mat &matBg = frame->get();
    float x = matBg.cols * position.x;
    float y = matBg.rows * position.y;
    cv::putText(frame->get(), text, cv::Point(x, y), cv::FONT_HERSHEY_SIMPLEX, 3, cv::Scalar(255, 255, 255));
}

void Screen::fill(const Point &point, const Size &size, const FramePtr topFrame, float beta)
{
    cv::Mat &matBg = frame->get();
    float minX = matBg.cols * point.x;
    float minY = matBg.rows * point.y;
    float w = matBg.cols * size.w;
    float h = matBg.rows * size.h;

    auto dest = matBg(cv::Rect(minX, minY, w, h));
    cv::Mat matTop = topFrame->get();
    cv::resize(matTop, matTop, cv::Size(w, h));
    float alpha = 1.0f - beta;
    cv::addWeighted(dest, alpha, matTop, beta, 0.0, dest);
}

void Screen::fill(const Point &point, const Size &size, unsigned char r, unsigned char g, unsigned char b)
{
    cv::Mat &mat = frame->get();
    float minX = mat.cols * point.x;
    float minY = mat.rows * point.y;
    float w = mat.cols * size.w;
    float h = mat.rows * size.h;
    cv::rectangle(mat, cv::Rect(minX, minY, w, h), cv::Scalar(r, g, b), CV_FILLED);
}

void Screen::fillCircle(const Point &point, const float size, unsigned char r, unsigned char g, unsigned char b)
{
    cv::Mat &mat = frame->get();
    float radius = mat.cols * size;
    cv::circle(mat, cv::Point(point.x * mat.cols, point.y * mat.rows), radius, cv::Scalar(r, g, b), CV_FILLED);
}

FramePtr Screen::getFrame()
{
    return frame;
}

const std::string &Screen::getName() const
{
    return name;
}

unsigned int Screen::getWidth() const
{
    return frame->get().cols;
}
unsigned int Screen::getHeight() const
{
    return frame->get().rows;
}
