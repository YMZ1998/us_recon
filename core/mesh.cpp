#include "mesh.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace us {
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
    file << "property float " << "x" << "\n";
    file << "property float " << "y" << "\n";
    file << "property float " << "z" << "\n";
    for (const auto& attrName : {"red","green","blue", "nx","ny","nz"}) {
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
}