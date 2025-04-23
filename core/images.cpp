#include "images.h"

#include <iostream>

#include "bmp_reader.h"

namespace us {
#define PI 3.1415926535
#define CENTER_X 120

void Image::Init(const std::vector<unsigned char>& header,
                 const std::vector<unsigned char>& image_data, int flag) {
  header_ = header;
  image_data_ = image_data;
  flag_ = flag;
  GetImageDimensions(header_.data(), width_, height_);

  ExtractCoords();
  two2three();
};

const void Image::info() const {
  std::cout << "flag: " << flag_ << std::endl;
  std::cout << "Image size: " << width_ << "," << height_ << std::endl;
}

const void Image::GetVertices(std::vector<Vertex>& vertices) const {
  for (unsigned int i = 0; i < coords_count_; ++i) {
    Vertex vertex;
    vertex.x = coords_[0][i];
    vertex.y = coords_[1][i];
    vertex.z = coords_[2][i];
    vertices.push_back(vertex);
  }
}

const void Image::PrintCoords() const {
  std::cout << "Coordinates of pixels with value 255 (max 50 points):\n";
  for (unsigned int i = 0; i < coords_count_; ++i) {
    std::cout << "(" << coords_[0][i] << ", " << coords_[1][i] << ", "
              << coords_[2][i] << ")\n";
  }
}

void Image::ExtractCoords() {
  coords_count_ = 0;
  for (unsigned int y = 0; y < height_; ++y) {
    for (unsigned int x = 0; x < width_; ++x) {
      unsigned int index = y * width_ + x;
      if (image_data_[index] == 255 && coords_count_ < 50) {
        coords_[0][coords_count_] = x;
        coords_[1][coords_count_] = y;
        coords_[2][coords_count_] = 0;
        coords_count_++;
      }
    }
  }
}

void Image::two2three() {
  double SIN = 0.0;
  double COS = 0.0;

  SIN = sin(PI * flag_ / 12);
  COS = cos(PI * flag_ / 12);
  for (unsigned int i = 0; i < 50; i++) {
    coords_[0][i] -= CENTER_X;
    coords_[2][i] = coords_[0][i] * SIN;
    coords_[0][i] = coords_[0][i] * COS;
  }
}

const us::Image& Images::GetImage(int n) const {
  if (n >= 0 && n < 12) {
    return images_[n];
  } else {
    throw std::out_of_range("Index out of range");
  }
}

const void Images::info() const {
  std::cout << "size: " << images_.size() << std::endl;
  for (const auto& img : images_) {
    img.info();
  }
}

}  // namespace us