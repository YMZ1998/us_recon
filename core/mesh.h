#pragma once
#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>

#include "us_recon_export.h"

namespace us {
struct Vertex {
  float x, y, z;
  Vertex() : x(0), y(0), z(0) {}
  Vertex(float x_val, float y_val, float z_val)
      : x(x_val), y(y_val), z(z_val) {}
  std::unordered_map<std::string, float> attributes;
};

struct Face {
  std::vector<int> vertex_indices;

  Face() { vertex_indices.resize(3); }

  Face(int idx1, int idx2, int idx3) {
    vertex_indices.resize(3);
    vertex_indices[0] = idx1;
    vertex_indices[1] = idx2;
    vertex_indices[2] = idx3;
  }
};

class us_recon_core_export Mesh {
 public:
  Mesh() = default;
  ~Mesh() = default;

  void AddVertices(const std::vector<Vertex>& vertices) {
    vertices_.insert(vertices_.end(), vertices.begin(), vertices.end());
  }

  void AddFace(const Face& face) { faces_.push_back(face); }

  bool Read(const std::string& filename);
  bool Write(const std::string& filename);

 public:
  std::vector<Vertex> vertices_;
  std::vector<Face> faces_;

 private:
  std::unordered_map<std::string, int> vertex_attributes_;
};

struct Edge {
  int v1, v2;
  Edge(int a, int b) {
    if (a < b) {
      v1 = a;
      v2 = b;
    } else {
      v1 = b;
      v2 = a;
    }
  }
  bool operator==(const Edge& other) const {
    return v1 == other.v1 && v2 == other.v2;
  }
};

struct EdgeHasher {
  std::size_t operator()(const Edge& e) const {
    return std::hash<int>()(e.v1) ^ std::hash<int>()(e.v2);
  }
};

class us_recon_core_export MeshSubdivision {
 public:
  static Mesh Subdivide(const Mesh& mesh);
};

}  // namespace us