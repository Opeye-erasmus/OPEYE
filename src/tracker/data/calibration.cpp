#include "calibration.h"
#include <dlib/optimization.h>


Calibration::Calibration()
    : projection()
{

}

void Calibration::addCalibrationPoint(float shouldX, float shouldY, float isX, float isY) {
    dlib::vector<double, 2> pFrom, pTo;

    pFrom.x() = isX;
    pFrom.y() = isY;

    pTo.x() = shouldX;
    pTo.y() = shouldY;

    fromPoints.push_back(pFrom);
    toPoints.push_back(pTo);


    if (fromPoints.size()>=4) {
        std::cout << "Calibrating." << std::endl;
        projection =  dlib::find_projective_transform(fromPoints, toPoints);
    }

}

Point Calibration::correct(const Point& input) const {
    dlib::vector<double, 2> pFrom, pTo;

    pFrom.x() = input.x;
    pFrom.y() = input.y;

    pTo = projection(pFrom);

    Point output;
    output.x = pTo.x();
    output.y = pTo.y();

    return output;
}
