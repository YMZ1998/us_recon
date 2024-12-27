#include <iostream>
#include <vector>

#include "bmp_reader.h"
#include "images.h"
#include <filesystem>

namespace fs = std::filesystem;

void ReadImage(const std::string& image_path, Image& image) {
  std::vector<unsigned char> header;
  std::vector<unsigned char> image_data;

  if (!ReadBMP(image_path, header,
               image_data)) {
    std::cerr << "Error: Failed to read the BMP file." << std::endl;
  }
  image.Init(header, image_data);
}

int main() {
  std::string file_path = "D:\\Code\\us_recon\\data\\PATI000";

  Images images;
  for (const auto& image_path : fs::directory_iterator(file_path)) {
    if (image_path.is_regular_file() && image_path.path().extension() == ".BMP") {
      std::cout << image_path.path() << "\n";
      Image image;
      ReadImage(image_path.path().string(), image);
      //image.info();
      images.PushImage(image);
    }
  }
  images.info();
  //Image image;
  //std::string image_path = "D:\\Code\\us_recon\\data\\PATI000\\01.BMP";
  //ReadImage(image_path, image);
  //image.info();

  //// 假设 BMP 数据是按行、按像素顺序存储的，并且每个像素为单字节（灰度值）
  //int pixelIndex = 0;
  //for (unsigned int y = 0; y < height; ++y) {
  //  for (unsigned int x = 0; x < width; ++x) {
  //    // 从灰度数据中提取单个像素值
  //    unsigned char gray = image_data[pixelIndex++];

  //    // 将灰度值存储到像素数组中
  //    image.pixels[y][x] = gray;  // 假设每个像素值是一个整数
  //  }
  //}

  // 显示图像信息

  //std::cout << "Image width: " << width << ", Image height: " << height
  //          << std::endl;
  //std::cout << "imageSize: "<< image_data.size() << std::endl;
  //std::cout << "headerSize: "<< header.size() << std::endl;


  //if (!WriteBMP("D:\\Code\\us_recon\\data\\01-out.BMP", header, image_data
  //             )) {
  //  std::cerr << "Error: Failed to write the BMP file." << std::endl;
  //  return 1;
  //}
  return 0;
}