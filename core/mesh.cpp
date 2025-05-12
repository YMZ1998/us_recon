#include "mesh.h"

#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace us {

#define M_PI 3.14159265358979323846

static Vertex computeFaceNormal(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
  Vertex edge1(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
  Vertex edge2(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
  return edge1.cross(edge2).normalize();
}

void Mesh::UpdateNormals() {
  for (auto& vertex : vertices_) {
    vertex.attributes["nx"] = 0.0f;
    vertex.attributes["ny"] = 0.0f;
    vertex.attributes["nz"] = 0.0f;
  }

  // 遍历每个面，计算面法线，并将其添加到顶点法线
  for (const auto& face : faces_) {
    const Vertex& v1 = vertices_[face.vertex_indices[0]];
    const Vertex& v2 = vertices_[face.vertex_indices[1]];
    const Vertex& v3 = vertices_[face.vertex_indices[2]];

    Vertex faceNormal = computeFaceNormal(v1, v2, v3);

    // 如果法线是零向量（退化的面），跳过
    if (faceNormal.x == 0.0f && faceNormal.y == 0.0f && faceNormal.z == 0.0f) {
      continue;
    }

    for (int i = 0; i < 3; ++i) {
      int vertex_idx = face.vertex_indices[i];
      vertices_[vertex_idx].attributes["nx"] += faceNormal.x;
      vertices_[vertex_idx].attributes["ny"] += faceNormal.y;
      vertices_[vertex_idx].attributes["nz"] += faceNormal.z;
    }
  }

  // 归一化所有顶点的法线
  for (auto& vertex : vertices_) {
    Vertex normal(vertex.attributes["nx"], vertex.attributes["ny"],
                  vertex.attributes["nz"]);
    normal = normal.normalize();
    vertex.attributes["nx"] = normal.x;
    vertex.attributes["ny"] = normal.y;
    vertex.attributes["nz"] = normal.z;
  }

  vertex_attributes_["nx"] = 1;
  vertex_attributes_["ny"] = 1;
  vertex_attributes_["nz"] = 1;
}

us::Mesh Mesh::Subdivide() const {
  std::vector<Vertex> new_vertices;
  new_vertices.reserve(vertices_.size() * 2);  // 预留一倍空间
  new_vertices.insert(new_vertices.end(), vertices_.begin(), vertices_.end());

  std::vector<Face> new_faces;
  std::unordered_map<Edge, int, EdgeHasher> edge_map;
  std::unordered_map<int, std::unordered_set<int>> vertex_neighbors;
  std::unordered_map<int, std::unordered_set<int>> vertex_faces;

  const auto& faces = faces_;
  const auto& vertices = vertices_;

  // 收集每个顶点的邻居和所属面
  for (size_t i = 0; i < faces.size(); ++i) {
    const auto& f = faces[i];
    int i0 = f.vertex_indices[0];
    int i1 = f.vertex_indices[1];
    int i2 = f.vertex_indices[2];

    vertex_faces[i0].insert(i);
    vertex_faces[i1].insert(i);
    vertex_faces[i2].insert(i);

    vertex_neighbors[i0].insert(i1);
    vertex_neighbors[i0].insert(i2);
    vertex_neighbors[i1].insert(i0);
    vertex_neighbors[i1].insert(i2);
    vertex_neighbors[i2].insert(i0);
    vertex_neighbors[i2].insert(i1);
  }

  auto get_or_create_edge_vertex = [&](int i1, int i2) -> int {
    Edge e(i1, i2);
    auto it = edge_map.find(e);
    if (it != edge_map.end())
      return it->second;

    const Vertex& v1 = vertices[i1];
    const Vertex& v2 = vertices[i2];
    Vertex new_v{};

    // 查找公共面
    std::unordered_set<int> common_faces;
    for (int f1 : vertex_faces[i1]) {
      if (vertex_faces[i2].count(f1))
        common_faces.insert(f1);
    }

    if (common_faces.size() == 2) {
      std::vector<int> opp_vertices;
      for (int face_id : common_faces) {
        for (int vi : faces[face_id].vertex_indices) {
          if (vi != i1 && vi != i2 &&
              std::find(opp_vertices.begin(), opp_vertices.end(), vi) ==
                  opp_vertices.end()) {
            opp_vertices.push_back(vi);
          }
        }
      }
      const Vertex& v3 = vertices[opp_vertices[0]];
      const Vertex& v4 = vertices[opp_vertices[1]];

      new_v.x = 3.0f / 8 * (v1.x + v2.x) + 1.0f / 8 * (v3.x + v4.x);
      new_v.y = 3.0f / 8 * (v1.y + v2.y) + 1.0f / 8 * (v3.y + v4.y);
      new_v.z = 3.0f / 8 * (v1.z + v2.z) + 1.0f / 8 * (v3.z + v4.z);
    } else {
      // 边界边
      new_v.x = 0.5f * (v1.x + v2.x);
      new_v.y = 0.5f * (v1.y + v2.y);
      new_v.z = 0.5f * (v1.z + v2.z);
    }

    int new_index = static_cast<int>(new_vertices.size());
    new_vertices.push_back(new_v);
    edge_map[e] = new_index;
    return new_index;
  };

  // 创建新面
  for (const auto& f : faces) {
    int i0 = f.vertex_indices[0];
    int i1 = f.vertex_indices[1];
    int i2 = f.vertex_indices[2];

    int a = get_or_create_edge_vertex(i0, i1);
    int b = get_or_create_edge_vertex(i1, i2);
    int c = get_or_create_edge_vertex(i2, i0);

    new_faces.emplace_back(i0, a, c);
    new_faces.emplace_back(i1, b, a);
    new_faces.emplace_back(i2, c, b);
    new_faces.emplace_back(a, b, c);
  }

  // 更新原始顶点位置
  for (size_t i = 0; i < vertices.size(); ++i) {
    auto it = vertex_neighbors.find(static_cast<int>(i));
    if (it == vertex_neighbors.end() || it->second.size() < 3)
      continue;

    int n = static_cast<int>(it->second.size());
    float beta =
        (5.0f / 8.0f -
         std::pow(3.0f + 2.0f * std::cos(2.0f * M_PI / n), 2) / 64.0f) /
        n;

    float sum_x = 0.0f, sum_y = 0.0f, sum_z = 0.0f;
    for (int j : it->second) {
      sum_x += vertices[j].x;
      sum_y += vertices[j].y;
      sum_z += vertices[j].z;
    }

    Vertex& v = new_vertices[i];
    v.x = (1 - n * beta) * vertices[i].x + beta * sum_x;
    v.y = (1 - n * beta) * vertices[i].y + beta * sum_y;
    v.z = (1 - n * beta) * vertices[i].z + beta * sum_z;
  }

  Mesh new_mesh;
  new_mesh.AddVertices(new_vertices);
  new_mesh.AddFaces(new_faces);
  return new_mesh;
}


bool Mesh::Read(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error opening file!" << std::endl;
    return false;
  }

  std::string line;
  bool inHeader = true;
  int vertexCount = 0;
  int faceCount = 0;
  bool hasColor = false;
  bool hasNormal = false;

  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string word;
    iss >> word;

    if (word == "element" && line.find("vertex") != std::string::npos) {
      iss >> word >> vertexCount;
    } else if (word == "element" && line.find("face") != std::string::npos) {
      iss >> word >> faceCount;
    } else if (word == "property") {
      std::string attributeType;
      iss >> attributeType;
      if (attributeType == "float" || attributeType == "uchar") {
        std::string attributeName;
        iss >> attributeName;
        vertex_attributes_[attributeName] = vertex_attributes_.size();
      }
    }

    if (line == "end_header") {
      inHeader = false;
      break;
    }
  }

  if (!inHeader) {
    vertices_.resize(vertexCount);
    for (int i = 0; i < vertexCount; ++i) {
      std::getline(file, line);
      std::istringstream iss(line);
      iss >> vertices_[i].x >> vertices_[i].y >> vertices_[i].z;
      for (const auto& attr : vertex_attributes_) {
        std::string attributeName = attr.first;
        if (attributeName == "red" || attributeName == "green" ||
            attributeName == "blue") {
          float colorValue;
          iss >> colorValue;
          vertices_[i].attributes[attributeName] = colorValue;
        }
        if (attributeName == "nx" || attributeName == "ny" ||
            attributeName == "nz") {
          float normalValue;
          iss >> normalValue;
          vertices_[i].attributes[attributeName] = normalValue;
        }
      }
    }

    faces_.resize(faceCount);
    for (int i = 0; i < faceCount; ++i) {
      std::getline(file, line);
      std::istringstream iss(line);
      int vertexCountInFace;
      iss >> vertexCountInFace;
      faces_[i].vertex_indices.resize(vertexCountInFace);
      for (int j = 0; j < vertexCountInFace; ++j) {
        iss >> faces_[i].vertex_indices[j];
      }
    }
  }

  file.close();
  return true;
}

bool Mesh::Write(const std::string& filename) {
  std::ofstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error opening file!" << std::endl;
    return false;
  }

  file << "ply\n";
  file << "format ascii 1.0\n";
  file << "element vertex " << vertices_.size() << "\n";
  file << "property float "
       << "x"
       << "\n";
  file << "property float "
       << "y"
       << "\n";
  file << "property float "
       << "z"
       << "\n";
  for (const auto& attrName : {"red", "green", "blue", "nx", "ny", "nz"}) {
    auto it = vertex_attributes_.find(attrName);

    if (it != vertex_attributes_.end()) {
      file << "property float " << attrName << "\n";
    }
  }

  file << "element face " << faces_.size() << "\n";
  file << "property list uchar int vertex_indices\n";
  file << "end_header\n";

  for (const auto& vertex : vertices_) {
    file << vertex.x << " " << vertex.y << " " << vertex.z;
    for (const auto& attr : vertex_attributes_) {
      if (vertex.attributes.find(attr.first) != vertex.attributes.end()) {
        file << " " << vertex.attributes.at(attr.first);
      }
    }
    file << "\n";
  }

  for (const auto& face : faces_) {
    file << face.vertex_indices.size();
    for (const auto& index : face.vertex_indices) {
      file << " " << index;
    }
    file << "\n";
  }

  file.close();
  std::cout << "save in: " << filename << std::endl;
  return true;
}

}  // namespace us