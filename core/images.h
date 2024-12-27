#pragma once
#include <iostream>
#include <vector>

#include "bmp_reader.h"

class Image {
 public:
  Image(){};
  ~Image(){};

  void Init(const std::vector<unsigned char>& header,
            const std::vector<unsigned char>& image_data) {
    header_ = header;
    image_data_ = image_data;
    GetImageDimensions(header_.data(), width_, height_);
  };

  void info() const {
    std::cout << "Image size: " << width_ << "," << height_ << "\n";
  }

 private:
  unsigned int width_ = 0;
  unsigned int height_ = 0;

  std::vector<unsigned char> header_;
  std::vector<unsigned char> image_data_;
};

class Images {
 public:
  Images() {}

  void PushImage(Image& image) { images_.push_back(image); };

  Image& GetImage(int n) {
    if (n >= 0 && n < 12) {
      return images_[n];
    } else {
      throw std::out_of_range("Index out of range");
    }
  }

  void info() const {
    std::cout << "size: " << images_.size() << std::endl;
    for (const auto& img : images_) {
      img.info();
    }
  }

 private:
  std::vector<Image> images_;
};