#pragma once
#include <string>
#include <vector>
#include "us_recon_export.h"

namespace us {
  // This header location indices are specified on the bitmap format specification (http://www.fastgraph.com/help/bmp_header_format.html)
  constexpr unsigned short HEADER_SIZE_INDEX = 10;
  constexpr unsigned short IMAGE_SIZE_INDEX = 34;
  constexpr unsigned short WIDTH_INDEX = 18;
  constexpr unsigned short HEIGHT_INDEX = 22;
  constexpr unsigned short BITS_PER_PIXEL_INDEX = 28;

  constexpr unsigned short BITS_PER_BYTE = 8;
  constexpr unsigned short HEADER_SIZE = 54;

  bool us_recon_core_export ReadBMP(const std::string& imagePath, std::vector<unsigned char>& header,
    std::vector<unsigned char>& rgbData);
  bool us_recon_core_export WriteBMP(const std::string& imagePath,
    const std::vector<unsigned char>& header,
    const std::vector<unsigned char>& rgbData);

  // 读取图像宽度和高度
  void us_recon_core_export GetImageDimensions(const unsigned char* header, unsigned int& width,
    unsigned int& height);
}