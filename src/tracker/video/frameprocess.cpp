
#include "frameprocess.h"

#include <dlib/opencv.h>

namespace
{
const std::string faceLandmarksPath = "shape_predictor_68_face_landmarks.dat";
}

FrameProcessor::FrameProcessor(unsigned int w, unsigned int h)
    : width(w), height(h), detector(dlib::get_frontal_face_detector()), sourceEyeMargin(20)
{
    dlib::deserialize(faceLandmarksPath) >> poseModel;
}

FramePtr FrameProcessor::operator()(FramePtr frame)
{
    LOCK(mutex);

    std::vector<std::vector<dlib::point>> eyeMarkers;
    cv::Mat src, res, result;
    result.create(width, height, CV_8UC3);

    cv::resize(frame.get()->get(), res, cv::Size(frame.get()->get().cols / 2, frame.get()->get().rows / 2));
    cv::medianBlur(res, src, 3);
    normalize(src);

    //    computeNormals(src, res);
    src.copyTo(res);

    if (getEyeMarkers(res, eyeMarkers))
    {
        cv::Mat res1;
        cv::Mat res2;
        auto eyeBounds1 = getEyeBounds(eyeMarkers[0]);
        auto eyeBounds2 = getEyeBounds(eyeMarkers[1]);
        cv::Mat tmp, tmp2;
        tmp2 = extract(res, eyeBounds1 / 2);
        cv::resize(tmp2, tmp, cv::Size(width / 2, height / 2));

        /// copy first eye top left
        tmp.copyTo(result.colRange(0, width / 2).rowRange(0, height / 2));

        tmp2 = extract(res, eyeBounds2 / 2);
        cv::resize(tmp2, tmp, cv::Size(width / 2, height / 2));

        /// copy second eye top right
        tmp.copyTo(result.colRange(width / 2, width).rowRange(0, height / 2));

        cv::resize(res, tmp, cv::Size(width / 2, height / 2));
        tmp.copyTo(result.colRange(0, width / 2).rowRange(height / 2, height));

        cv::addWeighted(result.colRange(width / 2, width).rowRange(0, height / 2),
                        0.5,
                        result.colRange(0, width / 2).rowRange(0, height / 2),
                        0.5, 0.0, result.colRange(width / 2, width).rowRange(height / 2, height));
    }
    else
    {
        return nullptr;
    }

    return FramePtr(new Frame(result));
}

cv::Mat FrameProcessor::extract(const cv::Mat &image, const dlib::rectangle &eyeRegion)
{
    auto minX = eyeRegion.left() - sourceEyeMargin;
    auto maxX = eyeRegion.right() + sourceEyeMargin;
    auto minY = eyeRegion.bottom() - sourceEyeMargin;
    auto maxY = eyeRegion.top() + sourceEyeMargin;
    return image(cv::Rect(minX, minY, maxX - minX, maxY - minY));
}

dlib::rectangle FrameProcessor::getEyeBounds(const std::vector<dlib::point> &markers) const
{
    long minX = 10000;
    long maxX = 0;
    long minY = 10000;
    long maxY = 0;

    //Walk through the "eye points" of face detctor.
    for (unsigned int i = 0; i < 6; ++i)
    {
        const auto &part = markers[i];
        const auto x = part.x();
        const auto y = part.y();
        if (x < minX)
            minX = x;
        if (y < minY)
            minY = y;
        if (x > maxX)
            maxX = x;
        if (y > maxY)
            maxY = y;
    }

    return dlib::rectangle(minX, maxY, maxX, minY);
}

bool FrameProcessor::getEyeMarkers(const cv::Mat &frame, std::vector<std::vector<dlib::point>> &eyeMarkers)
{

    static cv::Mat buffer;
    static const int scale = 2;
    static const float workingWidth = frame.cols / scale;
    static const float workingHeight = frame.rows / scale;
    const static cv::Size workingDimension(workingWidth, workingHeight);
    if (buffer.empty())
        buffer.create(workingDimension, CV_8UC3);

    cv::resize(frame, buffer, workingDimension);

    dlib::cv_image<dlib::bgr_pixel> cimg(buffer);

    // Detect face
    std::vector<dlib::rectangle> faces = detector(cimg);
    if (faces.size() != 1)
    {
        // std::cout << "No face" << std::endl;
        return false;
    }

    // Extract pose
    dlib::full_object_detection shape = poseModel(cimg, faces[0]);
    assert(shape.num_parts() == 68);

    for (unsigned int j = 0; j < 2; j++)
    {
        std::vector<dlib::point> eye;
        //Walk through the "eye points" of face detctor.
        for (unsigned int i = 0; i < 6; ++i)
        {
            unsigned int index = 36 + j * 6 + i;
            const auto &part = shape.part(index);
            eye.push_back(dlib::point(part.x() * scale, part.y() * scale));
        }
        eyeMarkers.push_back(eye);
    }

    return true;
}

void FrameProcessor::normalize(cv::Mat &image)
{
    float min = 1.0f;
    float max = 0.0f;
    // First pass
    for (unsigned int x = 0; x < image.cols; ++x)
    {
        for (unsigned int y = 0; y < image.rows; ++y)
        {
            // Load and convert
            cv::Vec3b &pixel = image.at<cv::Vec3b>(y, x);
            float r = static_cast<float>(pixel[0]) / 255.0f;
            float g = static_cast<float>(pixel[1]) / 255.0f;
            float b = static_cast<float>(pixel[2]) / 255.0f;
            auto intensity = std::sqrt(r * r + g * g + b * b) / 1.7320508075688;
            pixel[0] = intensity * 255.0f;
            pixel[1] = 0;
            pixel[2] = 0;
            if (min > intensity)
            {
                min = intensity;
            }
            if (max < intensity)
            {
                max = intensity;
            }
        }
    }

    float scale = 1.0f / (max - min);
    float offset = min;

    // Second pass
    for (unsigned int x = 0; x < image.cols; ++x)
    {
        for (unsigned int y = 0; y < image.rows; ++y)
        {
            cv::Vec3b &pixel = image.at<cv::Vec3b>(y, x);

            float intensity = static_cast<float>(pixel[0]) / 255.0f;
            intensity = ((intensity - offset) * scale);
            if (intensity < 0.0f)
                intensity = 0.0f;
            if (intensity > 1.0f)
                intensity = 1.0f;

            pixel[0] = intensity * 255.0f;
            pixel[1] = intensity * 255.0f;
            pixel[2] = intensity * 255.0f;
        }
    }
}

void FrameProcessor::computeNormals(const cv::Mat &input, cv::Mat &output)
{
    unsigned int range = 1;
    for (unsigned int x = 0; x < input.cols - range; ++x)
    {
        for (unsigned int y = 0; y < input.rows - range; ++y)
        {
            // Load and convert
            cv::Vec3f intensityA = input.at<cv::Vec3b>(y, x);
            cv::Vec3f intensityB = input.at<cv::Vec3b>(y, x + range);
            cv::Vec3f intensityC = input.at<cv::Vec3b>(y + range, x);

            float width = 11;

            intensityA = cv::Vec3f(0.0f, 0.0f, intensityA[0]);
            intensityB = cv::Vec3f(width, 0.0f, intensityB[0]);
            intensityC = cv::Vec3f(0.0f, width, intensityC[0]);

            auto diffA = intensityA - intensityB;
            auto diffB = intensityA - intensityC;

            // normalize befor cross will result in normalized cross(?)
            diffA = cv::normalize(diffA);
            diffB = cv::normalize(diffB);

            auto normal = diffA.cross(diffB);

            // Readapt to 8 bit unsigned image
            cv::Vec3f out = (cv::Vec3f(0.5f + normal[0] * 0.5f, 0.5f + normal[1] * 0.5f, 0.5f + normal[2] * 0.5f));
            output.at<cv::Vec3b>(y, x) = cv::Vec3f(out[2], out[1], out[0]) * 255.0f;
        }
    }
}
