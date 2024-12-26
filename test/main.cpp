#include <iostream>
#include <vector>

#include "bmp_reader.h"


int main() {
  std::vector<unsigned char> header;
  std::vector<unsigned char> rgbData;

  std::cout << "Reading the BMP file..." << std::endl;
  if (!ReadBMP("D:\\Code\\us_recon\\data\\PATI000\\01.BMP", header, rgbData)) {
    std::cerr << "Error: Failed to read the BMP file." << std::endl;
    return 1;
  }
  unsigned int width = 0;
  unsigned int height = 0;
  GetImageDimensions(header.data(), width, height);

  std::cout << "Image width: " << width << ", Image height: " << height
            << std::endl;
  std::cout << "imageSize: "<< rgbData.size() << std::endl;
  std::cout << "headerSize: "<< header.size() << std::endl;


  if (!WriteBMP("D:\\Code\\us_recon\\data\\01-out.BMP", header, rgbData
               )) {
    std::cerr << "Error: Failed to write the BMP file." << std::endl;
    return 1;
  }
  return 0;
}