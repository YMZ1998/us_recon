// Mesh.cpp
#include "mesh.h"
namespace us {
  // 读取PLY文件
  bool Mesh::read(const std::string& filename) {
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

    // 读取 PLY 文件头
    while (std::getline(file, line)) {
      std::istringstream iss(line);
      std::string word;
      iss >> word;

      if (word == "element" && line.find("vertex") != std::string::npos) {
        iss >> word >> vertexCount;
      }
      else if (word == "element" && line.find("face") != std::string::npos) {
        iss >> word >> faceCount;
      }
      else if (word == "property") {
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

    // 读取顶点数据
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

      // 读取面数据
      faces_.resize(faceCount);
      for (int i = 0; i < faceCount; ++i) {
        std::getline(file, line);
        std::istringstream iss(line);
        int vertexCountInFace;
        iss >> vertexCountInFace;
        faces_[i].vertexIndices.resize(vertexCountInFace);
        for (int j = 0; j < vertexCountInFace; ++j) {
          iss >> faces_[i].vertexIndices[j];
        }
      }
    }

    file.close();
    return true;
  }

  // 写入PLY文件
  bool Mesh::write(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
      std::cerr << "Error opening file!" << std::endl;
      return false;
    }

    file << "ply\n";
    file << "format ascii 1.0\n";
    file << "element vertex " << vertices_.size() << "\n";
    for (const auto& attrName : {"x", "y", "z", "red","green","blue", "nx","ny","nz"}) {
      auto it = vertex_attributes_.find(attrName);

      if (it != vertex_attributes_.end()) {
        file << "property float " << attrName << "\n";
      }
    }

    file << "element face " << faces_.size() << "\n";
    file << "property list uchar int vertex_indices\n";
    file << "end_header\n";

    // 写入顶点数据
    for (const auto& vertex : vertices_) {
      file << vertex.x << " " << vertex.y << " " << vertex.z;
      for (const auto& attr : vertex_attributes_) {
        if (vertex.attributes.find(attr.first) != vertex.attributes.end()) {
          file << " " << vertex.attributes.at(attr.first);
        }
      }
      file << "\n";
    }

    // 写入面数据
    for (const auto& face : faces_) {
      file << face.vertexIndices.size();
      for (const auto& index : face.vertexIndices) {
        file << " " << index;
      }
      file << "\n";
    }

    file.close();
    return true;
  }
}