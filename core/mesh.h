#pragma once
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
  std::vector<int> vertex_indices;
};

class us_recon_core_export Mesh {
 public:
  Mesh() = default;
  ~Mesh() = default;

  void AddVertices(const std::vector<Vertex>& vertices) {
    vertices_.insert(vertices_.end(), vertices.begin(), vertices.end());
  }

  bool Read(const std::string& filename);
  bool Write(const std::string& filename);

private:
  std::vector<Vertex> vertices_;
  std::vector<Face> faces_;
  std::unordered_map<std::string, int> vertex_attributes_;

};
}  // namespace us