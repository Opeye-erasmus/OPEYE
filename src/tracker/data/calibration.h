#pragma once

#include <defines.h>
#include <dlib/geometry.h>

/**
 * Class allowing a simple callibration
 * 
 * 
 * Feed a samples with of "should" vs. "is" positions and it can
 * apply a perspective distrotion trying to eliminate unprecissions on various factors.
 */
class Calibration
{

public:
  Calibration();
  void addCalibrationPoint(float shouldX, float shouldY, float isX, float isY);

  Point correct(const Point &input) const;

private:
  dlib::point_transform_projective projection;

  std::vector<dlib::vector<double, 2>> fromPoints;
  std::vector<dlib::vector<double, 2>> toPoints;
};
