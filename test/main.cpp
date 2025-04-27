#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "mesh.h"
#include "us_recon.h"
using namespace us;
namespace fs = std::filesystem;

static std::vector<std::vector<uint16_t>> read_bin_in_groups(
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

static std::vector<std::vector<Point2d>> create_xy_groups(
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

void test_case(std::string case_name) {
  std::string data_path = "D:\\Data\\³¬Éù\\20250207_82521\\20250207\\";
  std::string output_filename =
      "D:\\Code\\us_recon\\data\\output_by_bin_subdivided.ply";
  std::string file_path_x = data_path + case_name + "\\BDX.BIN";
  std::string file_path_y = data_path + case_name + "\\BDY.BIN";

  auto groups_x = read_bin_in_groups(file_path_x, 49);
  auto groups_y = read_bin_in_groups(file_path_y, 49);

  std::vector<std::vector<Point2d>> xy_groups = create_xy_groups(groups_x, groups_y);
  UsRecon us_recon;

  Mesh subdivided_mesh = us_recon.Run(xy_groups, true, true);
  subdivided_mesh.Write(output_filename);
}

int main() {
  auto start = std::chrono::high_resolution_clock::now();

  test_case("PATI000");

  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double, std::milli> duration = end - start;
  std::cout << "Execution time: " << duration.count() << " ms" << std::endl;

  return 0;
}
