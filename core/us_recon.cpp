#include "us_recon.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include "mesh.h"

namespace us {
#define M_PI 3.14159265358979323846

constexpr double IMAGE_WIDTH = 240;
constexpr double CENTER_X = 120;
constexpr double ANGLE_STEP = 15;
constexpr double PIXEL_SPACING = 0.04979;
constexpr int NUM_IMAGES = 12;
constexpr int NUM_POINTS = 49;

static void PrintPoint(const Point2d& point) {
  std::cout << "(" << point.x << ", " << point.y << ")\n";
}

static void PrintPoints(const std::vector<Point2d>& points,
                        const std::string& str = "") {
  std::cout << str << std::endl;
  for (const auto& point : points) {
    PrintPoint(point);
  }
  std::cout << std::endl;
}

static std::vector<Point2d> smooth_points(const std::vector<Point2d>& points,
                                          size_t window_size = 5,
                                          size_t iterations = 1) {
  std::vector<Point2d> smoothed_points = points;

  int num_points = smoothed_points.size();

  for (int iter = 0; iter < iterations; ++iter) {
    for (int i = 0; i < num_points; ++i) {
      int start = i - window_size / 2;
      int end = i + window_size / 2 + 1;

      Point2d sum;

      if (start < 0) {
        for (int j = 0; j < end; ++j) {
          sum = sum + smoothed_points[j];
        }
        for (int j = num_points + start; j < num_points; ++j) {
          sum = sum + smoothed_points[j];
        }
      } else if (end > num_points) {
        for (int j = start; j < num_points; ++j) {
          sum = sum + smoothed_points[j];
        }
        for (int j = 0; j < end - num_points; ++j) {
          sum = sum + smoothed_points[j];
        }
      } else {
        for (int j = start; j < end; ++j) {
          sum = sum + smoothed_points[j];
        }
      }

      smoothed_points[i] = sum / window_size;
      //std::cout << start << "," << end << std::endl;
      //PrintPoint(points[i]);
      //PrintPoint(smoothed_points[i] - points[i]);
    }
  }

  return smoothed_points;
}

static std::vector<Point2d> adjust_spacing(const std::vector<Point2d>& points) {
  std::vector<double> distances = {0};
  double total_distance = 0;

  // 计算点集的总长度
  for (size_t i = 1; i < points.size(); ++i) {
    double distance = points[i].distance_to(points[i - 1]);
    total_distance += distance;
    distances.push_back(total_distance);
  }

  // 生成均匀间隔的目标位置
  std::vector<double> uniform_positions;
  size_t num_points = points.size();
  for (size_t i = 0; i < num_points; ++i) {
    uniform_positions.push_back(total_distance * i / (num_points - 1));
  }

  // 基于原始距离和目标距离，进行插值
  std::vector<Point2d> adjusted_points;
  for (size_t i = 0; i < num_points; ++i) {
    double target_pos = uniform_positions[i];
    // 查找最接近的插值位置
    auto insert_idx =
        std::lower_bound(distances.begin(), distances.end(), target_pos) -
        distances.begin();
    if (insert_idx == distances.size() - 1) {
      adjusted_points.push_back(points.back());
    } else {
      double t = (target_pos - distances[insert_idx - 1]) /
                 (distances[insert_idx] - distances[insert_idx - 1]);
      adjusted_points.push_back(
          Point2d::interpolate(points[insert_idx - 1], points[insert_idx], t));
    }
  }
  return adjusted_points;
}

static Point2d calculate_center(const std::vector<Point2d>& points) {
  size_t num_points = points.size();
  if (num_points == 0)
    return Point2d(0, 0);

  double sum_x = 0, sum_y = 0;

  for (const auto& point : points) {
    sum_x += point.x;
    sum_y += point.y;
  }

  return Point2d(sum_x / num_points, sum_y / num_points);
}

static void sort_points(std::vector<Point2d>& points) {
  if (points.size() < 2)
    return;

  Point2d center = calculate_center(points);
  center.x = CENTER_X;  // 固定中心 x 坐标

  std::sort(
      points.begin(), points.end(), [&](const Point2d& a, const Point2d& b) {
        double angle_a = std::atan2(a.y - center.y, a.x - center.x) + M_PI / 2;
        double angle_b = std::atan2(b.y - center.y, b.x - center.x) + M_PI / 2;

        if (angle_a < 0)
          angle_a += 2 * M_PI;
        if (angle_b < 0)
          angle_b += 2 * M_PI;

        return angle_a > angle_b;
      });

  if (std::abs(points[0].x - center.x) > std::abs(points.back().x - center.x)) {
    std::rotate(points.rbegin(), points.rbegin() + 1, points.rend());
  }
}

static Point3d convert_to_3d(Point2d point, double angle_deg, double center_x,
                             double spacing) {
  double x_offset = (point.x - center_x) * spacing;
  double y = point.y * spacing;
  double angle_rad = angle_deg * M_PI / 180.0;
  double x = x_offset * std::cos(angle_rad);
  double z = x_offset * std::sin(angle_rad);
  return Point3d(x, y, z);
}

static std::vector<std::vector<Point3d>> merge_3d_points(
    std::vector<std::vector<Point3d>>& points) {
  std::vector<std::vector<Point3d>> new_points;

  Point3d first_point = points[0][0];
  Point3d middle_point = points[0][points[0].size() / 2];

  // Calculate the average of the first and middle points
  for (size_t i = 1; i < points.size(); ++i) {
    first_point = first_point + points[i][0];
    middle_point = middle_point + points[i][points[i].size() / 2];
  }

  // Average the first and middle points
  first_point = first_point / points.size();
  middle_point = middle_point / points.size();

  int middle_index = points[0].size() / 2;
  // Modify the groups with the averaged first and middle points
  for (auto& group : points) {
    group[0] = first_point;
    group[1] = group[0];
    group[group.size() - 1] = group[0];
    group[middle_index] = middle_point;
    group[middle_index - 1] = group[middle_index];
    group[middle_index + 1] = group[middle_index];

    new_points.push_back(group);
  }

  return new_points;
}

static Mesh generate_mesh(const std::vector<std::vector<Point3d>>& all_rings_3d,
                          bool subdivide = true, bool update_normal = true) {
  Mesh mesh;
  for (const auto& ring_3d : all_rings_3d) {
    std::vector<Vertex> vertices;
    for (const auto& point : ring_3d) {
      vertices.push_back(Vertex(point.x, point.y, point.z));
    }
    mesh.AddVertices(vertices);
  }
  std::vector<Face> triangles;
  int vert_start_idx = 0;

  // Build triangles between consecutive rings
  for (size_t i = 0; i < all_rings_3d.size() - 1; ++i) {
    const auto& ring1 = all_rings_3d[i];
    const auto& ring2 = all_rings_3d[i + 1];
    size_t min_len = std::min(ring1.size(), ring2.size());

    for (size_t j = 0; j < min_len - 1; ++j) {
      int idx1 = vert_start_idx + j;
      int idx2 = vert_start_idx + j + 1;
      int idx3 = vert_start_idx + ring1.size() + j;
      int idx4 = vert_start_idx + ring1.size() + j + 1;

      triangles.push_back({idx1, idx2, idx3});
      triangles.push_back({idx2, idx4, idx3});
    }

    // Closing the ring
    int last_idx_ring1 = vert_start_idx + ring1.size() - 1;
    int first_idx_ring1 = vert_start_idx;
    int last_idx_ring2 = vert_start_idx + ring1.size() + ring2.size() - 1;
    int first_idx_ring2 = vert_start_idx + ring1.size();

    triangles.push_back({last_idx_ring1, first_idx_ring1, last_idx_ring2});
    triangles.push_back({first_idx_ring1, first_idx_ring2, last_idx_ring2});

    vert_start_idx += ring1.size();
  }

  // Connecting the last ring with the first ring
  const auto& ring1 = all_rings_3d.back();
  const auto& ring2 = all_rings_3d.front();
  int offset1 = 0;
  for (size_t i = 0; i < all_rings_3d.size() - 1; ++i) {
    offset1 += all_rings_3d[i].size();
  }
  int offset2 = 0;
  size_t min_len = std::min(ring1.size(), ring2.size());

  for (size_t j = 0; j < min_len - 1; ++j) {
    int idx1 = offset1 + j;
    int idx2 = offset1 + j + 1;
    int idx3 = offset2 + ring2.size() - j - 1;
    int idx4 = offset2 + ring2.size() - j - 2;

    triangles.push_back({idx1, idx2, idx3});
    triangles.push_back({idx2, idx4, idx3});
  }

  int last_idx_ring1 = offset1 + ring1.size() - 1;
  int first_idx_ring1 = offset1;
  int last_idx_ring2 = offset2 + ring2.size() - 1;
  int first_idx_ring2 = offset2;

  triangles.push_back({last_idx_ring1, first_idx_ring1, last_idx_ring2});
  triangles.push_back({first_idx_ring1, first_idx_ring2, last_idx_ring2});

  mesh.AddFaces(triangles);
  Mesh subdivided_mesh = subdivide ? mesh.Subdivide() : mesh;
  if (update_normal) {
    subdivided_mesh.UpdateNormals();
  }
  return subdivided_mesh;
}

static int find_nearest_point_near_index(const std::vector<Point2d>& group,
                                         double target_x = 120.0) {
  int middle_index = group.size() / 2;

  int start = std::max(0, middle_index - 8);
  int end = std::min((int)group.size(), middle_index + 8);

  double min_distance = std::numeric_limits<double>::max();
  int nearest_index = start;

  for (int i = start; i < end; ++i) {
    double distance = std::abs(group[i].x - target_x);
    if (distance < min_distance) {
      min_distance = distance;
      nearest_index = i;
    }
  }

  return nearest_index;
}

void adjust_points_with_copy_remove(std::vector<Point2d>& points) {
  int nearest_index = find_nearest_point_near_index(points);
  //PrintPoint(points[nearest_index]);

  int left_count = nearest_index;
  int right_count = points.size() - nearest_index - 1;

  if (left_count < right_count) {
    while (left_count < right_count) {
      Point2d copy_point = points[nearest_index - 1];  // 当前的左边点
      points.insert(points.begin() + nearest_index, copy_point);
      nearest_index += 1;
      points.erase(points.begin() + nearest_index + 1);  // 删除右边点
      left_count += 1;
      right_count -= 1;
    }
  } else if (right_count < left_count) {
    while (right_count < left_count) {
      points.erase(points.begin() + nearest_index - 1);  // 删除左边点
      nearest_index -= 1;
      Point2d copy_point = points[nearest_index + 1];  // 当前的右边点
      points.insert(points.begin() + nearest_index + 1, copy_point);
      right_count += 1;
      left_count -= 1;
    }
  }

  int middle_index = points.size() / 2;
  //PrintPoint(points[middle_index]);
}

void adjust_spacing_half(std::vector<Point2d>& points_2d) {
  int middle_index = points_2d.size() / 2;

  std::vector<Point2d> left(points_2d.begin(),
                            points_2d.begin() + middle_index);
  std::vector<Point2d> right(points_2d.begin() + middle_index, points_2d.end());

  left = adjust_spacing(left);
  right = adjust_spacing(right);

  for (int i = 0; i < middle_index; ++i) {
    points_2d[i] = left[i];
  }
  for (int i = 0; i < right.size(); ++i) {
    points_2d[middle_index + i] = right[i];
  }
}

Mesh UsRecon::Run(std::vector<std::vector<Point2d>> xy_groups, bool subdivide,
                  bool update_normal) {
  std::vector<std::vector<Point3d>> all_rings_3d;
  all_rings_3d.reserve(NUM_IMAGES);

  for (int i = 0; i < NUM_IMAGES; ++i) {
    double angle_deg = i * ANGLE_STEP;
    std::vector<Point2d> points_2d = xy_groups[i];

    while (points_2d.size() < NUM_POINTS) {
      points_2d.push_back(points_2d.back());
    }

    if (std::all_of(points_2d.begin(), points_2d.end(),
                    [](const Point2d& p) { return p.x == 0 && p.y == 0; })) {
      std::cout << "No points found in : " << i << std::endl;
      continue;
    }

    sort_points(points_2d);
    points_2d = adjust_spacing(points_2d);
    points_2d = smooth_points(points_2d);
    adjust_points_with_copy_remove(points_2d);
    adjust_spacing_half(points_2d);

    std::vector<Point3d> points_3d;
    points_3d.reserve(NUM_POINTS);
    for (const auto& point : points_2d) {
      points_3d.push_back(
          convert_to_3d(point, angle_deg, CENTER_X, PIXEL_SPACING));
    }
    all_rings_3d.push_back(points_3d);
  }

  std::vector<std::vector<Point3d>> updated_points =
      merge_3d_points(all_rings_3d);

  return generate_mesh(updated_points, subdivide, update_normal);
}

}  // namespace us
