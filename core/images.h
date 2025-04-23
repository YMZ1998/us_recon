#pragma once
#include <vector>

#include "mesh.h"
#include "us_recon_export.h"

namespace us {
class us_recon_core_export Image {
 public:
  Image(){};
  ~Image(){};

  void Init(const std::vector<unsigned char>& header,
            const std::vector<unsigned char>& image_data, int flag);

  const void info() const;

  const void GetVertices(std::vector<Vertex>& vertices) const;

  const void PrintCoords() const;

 private:
  void ExtractCoords();
  void two2three();

 private:
  int flag_ = 0;
  unsigned int width_ = 0;
  unsigned int height_ = 0;

  std::vector<unsigned char> header_;
  std::vector<unsigned char> image_data_;

  double coords_[3][50] = {};
  unsigned int coords_count_ = 0;
};

class us_recon_core_export Images {
 public:
  Images() { images_.reserve(12); }
  ~Images() {}

  void PushImage(Image& image) { images_.push_back(image); };

  const Image& GetImage(int n) const;

  const void info() const;

 private:
  std::vector<Image> images_;
};
}  // namespace us