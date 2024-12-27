#include <iostream>
#include <vector>

#include <filesystem>
#include "bmp_reader.h"
#include "images.h"

namespace fs = std::filesystem;

void ReadImage(const std::string& image_path, Image& image, int flag) {
  std::vector<unsigned char> header;
  std::vector<unsigned char> image_data;

  if (!ReadBMP(image_path, header, image_data)) {
    std::cerr << "Error: Failed to read the BMP file." << std::endl;
  }
  image.Init(header, image_data, flag);
}

int main() {
  std::string file_path = "D:\\Code\\us_recon\\data\\test";

  Images images;
  int flag = 0;
  for (const auto& image_path : fs::directory_iterator(file_path)) {
    if (image_path.is_regular_file() &&
        image_path.path().extension() == ".BMP") {
      std::cout << image_path.path() << "\n";
      Image image;
      ReadImage(image_path.path().string(), image, flag++);
      image.PrintCoords();
      images.PushImage(image);
    }
  }
  images.info();

  return 0;
}