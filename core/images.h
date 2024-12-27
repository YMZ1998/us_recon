#pragma once
#include <iostream>
#include <vector>

#include "bmp_reader.h"

namespace us {
#define PI 3.1416
#define CENTER_X 200

  class Image {
  public:
    Image() {};
    ~Image() {};

    void Init(const std::vector<unsigned char>& header,
      const std::vector<unsigned char>& image_data, int flag) {
      header_ = header;
      image_data_ = image_data;
      flag_ = flag;
      GetImageDimensions(header_.data(), width_, height_);

      ExtractCoords();
      two2three();
    };

    void info() const {
      std::cout << "flag: " << flag_ << std::endl;
      std::cout << "Image size: " << width_ << "," << height_ << "\n";
    }

    void get_coords(double coords[50][3]) const {
      for (unsigned int i = 0; i < coords_count_; ++i) {
        coords[i][0] = coords_[i][0];
        coords[i][1] = coords_[i][1];
        coords[i][2] = coords_[i][2];
      }
    }


    void ExtractCoords() {
      coords_count_ = 0;
      for (unsigned int y = 0; y < height_; ++y) {
        for (unsigned int x = 0; x < width_; ++x) {
          unsigned int index = y * width_ + x;
          if (image_data_[index] == 255 &&
            coords_count_ < 50) {
            coords_[0][coords_count_] = x;
            coords_[1][coords_count_] = y;
            coords_[2][coords_count_] = 0;
            coords_count_++;
          }
        }
      }
    }

    void two2three() {
      double SIN = 0.0;
      double COS = 0.0;

      SIN = sin(PI * flag_ / 12);
      COS = cos(PI * flag_ / 12);
      for (unsigned int i = 0; i < 50; i++)
      {
        coords_[0][i] -= CENTER_X;
        coords_[2][i] = coords_[0][i] * SIN;
        coords_[0][i] = coords_[0][i] * COS;
      }
    }


    void PrintCoords() const {
      std::cout << "Coordinates of pixels with value 255 (max 50 points):\n";
      for (unsigned int i = 0; i < coords_count_; ++i) {
        std::cout << "(" << coords_[0][i] << ", " << coords_[1][i] << ", "
          << coords_[2][i] << ")\n";
      }
    }

  private:
    int flag_ = 0;
    unsigned int width_ = 0;
    unsigned int height_ = 0;

    std::vector<unsigned char> header_;
    std::vector<unsigned char> image_data_;

    double coords_[3][50] = {};
    unsigned int coords_count_ = 0;
  };

  class Images {
  public:
    Images() {}
    ~Images() {}

    void PushImage(Image& image) { images_.push_back(image); };

    Image& GetImage(int n) {
      if (n >= 0 && n < 12) {
        return images_[n];
      }
      else {
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
}