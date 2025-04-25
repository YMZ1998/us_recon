#pragma once
#include <cmath>
#include <vector>

#include "us_recon_export.h"

namespace us {

struct Point3d {
  double x;
  double y;
  double z;
  Point3d() : x(0), y(0), z(0) {}
  Point3d(double x_val, double y_val, double z_val)
      : x(x_val), y(y_val), z(z_val) {}

  Point3d operator+(const Point3d& p) const {
    return Point3d(x + p.x, y + p.y, z + p.z);
  }

  Point3d operator/(double val) const {
    return Point3d(x / val, y / val, z / val);
  }
};

struct Point2d {
  double x, y;
  Point2d() : x(0), y(0) {}
  Point2d(double x_val, double y_val) : x(x_val), y(y_val) {}

  double distance_to(const Point2d& other) const {
    return std::sqrt(std::pow(x - other.x, 2) + std::pow(y - other.y, 2));
  }

  static Point2d interpolate(const Point2d& p1, const Point2d& p2, double t) {
    double x_new = p1.x + t * (p2.x - p1.x);
    double y_new = p1.y + t * (p2.y - p1.y);
    return Point2d(x_new, y_new);
  }
};

class us_recon_core_export UsRecon {
 public:
  UsRecon() {}
  ~UsRecon() {}

  void Run(std::vector<std::vector<Point2d>> xy_groups);

 private:
  ;
};
}  // namespace us