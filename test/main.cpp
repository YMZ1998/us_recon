#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "mesh.h"
using namespace us;
namespace fs = std::filesystem;

#define M_PI 3.1415926535

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

  Point3d operator/(double val) const { return Point3d(x / val, y / val, z / val); }
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

std::vector<std::vector<uint16_t>> read_bin_in_groups(
    const std::string& file_path, size_t group_size = 49,
    size_t discard_size = 2, size_t num_groups = 12) {
  std::ifstream file(file_path, std::ios::binary);

  if (!file.is_open()) {
    std::cerr << "file not found: " << file_path << std::endl;
    return {};
  }

  std::vector<uint16_t> data;
  uint16_t value;
  while (file.read(reinterpret_cast<char*>(&value), sizeof(value))) {
    data.push_back(value);
  }

  std::vector<std::vector<uint16_t>> groups;
  size_t index = 0;

  for (size_t i = 0; i < num_groups; ++i) {
    if (index + group_size <= data.size()) {
      std::vector<uint16_t> group(data.begin() + index,
                                  data.begin() + index + group_size);
      groups.push_back(group);
      index += group_size + discard_size;
    } else {
      break;
    }
  }

  return groups;
}

std::vector<std::vector<Point2d>> create_xy_groups(
    const std::vector<std::vector<uint16_t>>& groups_x,
    const std::vector<std::vector<uint16_t>>& groups_y) {
  std::vector<std::vector<Point2d>> xy_groups;

  for (size_t i = 0; i < groups_x.size(); ++i) {
    const auto& x_group = groups_x[i];
    const auto& y_group = groups_y[i];

    if (!x_group.empty() && !y_group.empty() &&
        x_group.size() == y_group.size()) {
      std::vector<Point2d> xy_group;
      for (size_t j = 0; j < x_group.size(); ++j) {
        xy_group.emplace_back(y_group[j] / 2.0f, x_group[j] / 2.0f);
      }
      xy_groups.push_back(xy_group);
    }
  }

  return xy_groups;
}

std::vector<Point2d> adjust_spacing(const std::vector<Point2d>& sorted_points) {
  std::vector<Point2d> points = sorted_points;
  std::vector<double> distances = {0};  // 起始点的距离为 0
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
  std::cout << "调整后的点集：" << std::endl;
  for (const auto& point : adjusted_points) {
    std::cout << "(" << point.x << ", " << point.y << ") ";
  }
  std::cout << std::endl;
  return adjusted_points;
}

Point2d calculate_center(const std::vector<Point2d>& points) {
  double sum_x = 0, sum_y = 0;
  for (const auto& point : points) {
    sum_x += point.x;
    sum_y += point.y;
  }
  return Point2d(sum_x / points.size(), sum_y / points.size());
}

// 按照极角排序点
std::vector<Point2d> sort_points(std::vector<Point2d>& points) {
  std::cout << "开始排序点数: " << points.size() << std::endl;
  std::cout << "排序前的点集：" << std::endl;
  for (const auto& point : points) {
    std::cout << "(" << point.x << ", " << point.y << ") ";
  }
  std::cout << std::endl;

  if (points.size() < 3) {
    return points;  // 无需排序
  }

  // 计算重心，默认中点为所有点的均值
  Point2d center = calculate_center(points);
  center.x = 120;  // 设置 x 坐标为 120
  std::cout << center.y << std::endl;

  // 计算每个点的相对坐标并计算极角
  std::vector<std::pair<double, size_t>> angles;  // 保存角度和原始索引
  for (size_t i = 0; i < points.size(); ++i) {
    double dx = points[i].x - center.x;
    double dy = points[i].y - center.y;

    double angle = std::atan2(dy, dx);
    angle += M_PI / 2;  // 以90度为起点

    // 确保角度在 [0, 2π) 范围内
    if (angle < 0) {
      angle += 2 * M_PI;
    }
    angles.push_back({angle, i});
  }

  // 按照角度排序，逆时针方向
  std::sort(
      angles.begin(), angles.end(),
      [](const std::pair<double, size_t>& a,
         const std::pair<double, size_t>& b) { return a.first > b.first; });

  std::vector<Point2d> sorted_points;
  for (const auto& angle : angles) {
    sorted_points.push_back(points[angle.second]);
  }

  if (std::abs(sorted_points[0].x - center.x) >
      std::abs(sorted_points.back().x - center.x)) {
    std::rotate(sorted_points.rbegin(), sorted_points.rbegin() + 1,
                sorted_points.rend());
  }

  std::cout << "排序后的点集：" << std::endl;
  for (const auto& point : sorted_points) {
    std::cout << "(" << point.x << ", " << point.y << ") ";
  }
  std::cout << std::endl;

  return sorted_points;
}

Point3d convert_to_3d(Point2d point, double angle_deg, double center_x,
                      double spacing) {
  double x_offset = (point.x - center_x) * spacing;
  double y = point.y * spacing;
  double angle_rad = angle_deg * M_PI / 180.0;
  double x = x_offset * std::cos(angle_rad);
  double z = x_offset * std::sin(angle_rad);
  return Point3d(x, y, z);
}

std::vector<std::vector<Point3d>> replace_first_last_with_average(
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

  // Modify the groups with the averaged first and middle points
  for (auto& group : points) {
    group[0] = first_point;
    group[1] = first_point;
    group[group.size() - 1] = first_point;
    group[group.size() / 2] = middle_point;
    if (group.size() > 1) {
      group[group.size() / 2 - 1] = middle_point;
    }
    if (group.size() > 2) {
      group[group.size() / 2 + 1] = middle_point;
    }

    new_points.push_back(group);
  }

  return new_points;
}

void generate_mesh(const std::vector<std::vector<Point3d>>& all_rings_3d) {
  Mesh mesh;
  for (const auto& ring_3d : all_rings_3d) {
    std::vector<Vertex> vertices;
    for (const auto& point : ring_3d) {
      vertices.push_back(Vertex(point.x, point.y, point.z));
    }
    mesh.AddVertices(vertices);
  }
  std::vector<std::vector<int>> triangles;
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

  for (const auto& triangle : triangles) {
    Face face(triangle[0], triangle[1], triangle[2]);
    mesh.AddFace(face);
  }

  mesh.Write("D:\\Code\\us_recon\\data\\output_by_bin.ply");
}

int main() {
  std::string case_name = "PATI035";
  std::string data_path = "D:\\Data\\超声\\20250207_82521\\20250207\\";
  std::string file_path_x = data_path + case_name + "\\BDX.BIN";
  std::string file_path_y = data_path + case_name + "\\BDY.BIN";

  auto groups_x = read_bin_in_groups(file_path_x, 49);
  auto groups_y = read_bin_in_groups(file_path_y, 49);

  std::vector<std::vector<Point2d>> xy_groups =
      create_xy_groups(groups_x, groups_y);

  // 对每组点进行排序
  //std::vector<std::vector<Point2d>> sorted_xy_groups;
  //for (const auto& group : xy_groups) {
  //  std::vector<Point2d> points;
  //  for (const auto& point : group) {
  //    points.push_back(point);
  //  }
  //  // 对每组点进行排序
  //  std::vector<Point2d> sorted_points = sort_points(points);
  //  std::vector<Point2d> adjusted_points = adjust_spacing(sorted_points);
  //  sorted_xy_groups.push_back(adjusted_points);
  //}

  // 设置常量
  const double IMAGE_WIDTH = 240;
  const double ANGLE_STEP = 15;
  const double PIXEL_SPACING = 0.04979;
  const int NUM_IMAGES = 12;
  std::vector<std::vector<Point2d>> all_rings_2d;
  std::vector<std::vector<Point3d>> all_rings_3d;

  for (int i = 0; i < NUM_IMAGES; ++i) {
    double angle_deg = i * ANGLE_STEP;
    double center_x = IMAGE_WIDTH / 2;
    std::vector<Point2d> points_2d = xy_groups[i];

    while (points_2d.size() < 49) {
      points_2d.push_back(points_2d.back());
    }

    if (std::all_of(points_2d.begin(), points_2d.end(),
                    [](const Point2d& p) { return p.x == 0 && p.y == 0; })) {
      std::cout << "No points found in image: " << i << std::endl;
      continue;
    }

    // 排序、调整间距
    points_2d = sort_points(points_2d);
    points_2d = adjust_spacing(points_2d);

    std::vector<Point3d> points_3d;
    for (const auto& point : points_2d) {
      points_3d.push_back(
          convert_to_3d(point, angle_deg, center_x, PIXEL_SPACING));
    }

    all_rings_2d.push_back(points_2d);
    all_rings_3d.push_back(points_3d);
  }

  std::vector<std::vector<Point3d>> updated_points = replace_first_last_with_average(all_rings_3d);

  generate_mesh(updated_points);

  return 0;
}
