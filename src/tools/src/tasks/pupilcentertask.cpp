#include "pupilcentertask.h"
#include "video/frameprocess.h"
#include <sstream>
#include <vector>

cv::Mat image[5];
FramePtr frame[5];
cv::Mat nor[5];
FramePtr norFrame[5];
cv::Mat der[5];
FramePtr derFrame[5];

PupilCenterTask::PupilCenterTask()
{
/*    image[0] = cv::imread("data/img_1.jpg");
    auto rect = cv::Rect(801,684,88,66);
    image[0] = image[0](rect);

    for (int i = 0;i < 5;++i) {
        if (i > 0) {
            cv::resize(image[0], image[i], cv::Size(image[i-1].cols/2, image[i-1].rows/2), 0.0, 0.0, cv::INTER_LANCZOS4);
        }
        frame[i] = FramePtr(new Frame(image[i]));


        cv::medianBlur(image[i], nor[i], 7);
        FrameProcessor::normalize(nor[i]);
        norFrame[i] = FramePtr(new Frame(nor[i]));

        std::vector<cv::Vec3f> circles;

        /// Apply the Hough Transform to find the circles
        cv::Mat gray;
        cvtColor( nor[i], gray, CV_BGR2GRAY );
        cv::HoughCircles(gray, circles, cv::HOUGH_PROBABILISTIC, 1, 1);
        for (auto c : circles){

            cv::Point center(c[0], c[1]);
            int radius = c[2];

            auto color = cv::Scalar(255,0,0);
            cv::circle(nor[i], center, radius, color, -1, 8, 0);
        }

        nor[i].copyTo(der[i]);
        FrameProcessor::computeNormals(nor[i],der[i]);
        derFrame[i] = FramePtr(new Frame(der[i]));
    }*/


}

PupilCenterTask::~PupilCenterTask() {

}

void PupilCenterTask::keyPress(Key key) {

}

void PupilCenterTask::update(float delta) {

}

void PupilCenterTask::render(Screen& screen) {
    //screen.draw(frame)
    for (int i = 0; i < 45; ++i) {
        screen.fill({0.0f+0.2f*i, 0.0f}, {0.2f, 0.2f}, frame[i]);
        screen.fill({0.0f+0.2f*i, 0.2f}, {0.2f, 0.2f}, norFrame[i]);
        screen.fill({0.0f+0.2f*i, 0.4f}, {0.2f, 0.2f}, derFrame[i]);
    }

}
