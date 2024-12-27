#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "us_recon_export.h"

namespace us {
struct Vertex {
  float x, y, z;
  std::unordered_map<std::string, float>
      attributes;
};

struct Face {
  std::vector<int> vertexIndices;
};

class us_recon_core_export Mesh {
 public:
  Mesh() = default;
  ~Mesh() = default;

  bool read(const std::string& filename);
  bool write(const std::string& filename);

private:
  std::vector<Vertex> vertices_;
  std::vector<Face> faces_;
  std::unordered_map<std::string, int> vertex_attributes_;

};
}  // namespace us