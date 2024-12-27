#include <algorithm>
#include <iostream>
#include <vector>

#include "bmp_reader.h"
namespace us {
  bool ReadBMP(const std::string& imagePath, std::vector<unsigned char>& header,
    std::vector<unsigned char>& rgbData) {
    FILE* file = fopen(imagePath.c_str(), "rb");
    if (!file) {
      std::cerr << "Error: Could not open image file " << imagePath << std::endl;
      return false;
    }

    header.resize(HEADER_SIZE);
    if (fread(header.data(), 1, HEADER_SIZE, file) != HEADER_SIZE ||
      header[0] != 'B' || header[1] != 'M') {
      std::cerr << "Error: This is not a valid BMP file" << std::endl;
      fclose(file);
      return false;
    }

    unsigned int header_size =
      *reinterpret_cast<unsigned int*>(&header[HEADER_SIZE_INDEX]);
    unsigned int image_size =
      *reinterpret_cast<unsigned int*>(&header[IMAGE_SIZE_INDEX]);
    unsigned int width = *reinterpret_cast<unsigned int*>(&header[WIDTH_INDEX]);
    unsigned int height = *reinterpret_cast<unsigned int*>(&header[HEIGHT_INDEX]);
    unsigned short bits_per_pixel =
      *reinterpret_cast<unsigned short*>(&header[BITS_PER_PIXEL_INDEX]);

    if (header_size > HEADER_SIZE) {
      header.resize(header_size);
      rewind(file);
      if (fread(header.data(), 1, header_size, file) != header_size) {
        std::cerr << "Error: Failed to read the extended BMP header" << std::endl;
        fclose(file);
        return false;
      }
    }

    if (header_size == 0) {
      header_size = HEADER_SIZE;
    }

    if (image_size == 0) {
      image_size = width * height * (bits_per_pixel / BITS_PER_BYTE);
    }

    rgbData.resize(image_size);
    if (fread(rgbData.data(), 1, image_size, file) != image_size) {
      std::cerr << "Error: Failed to read BMP image data" << std::endl;
      fclose(file);
      return false;
    }

    fclose(file);
    return true;
  }

  bool WriteBMP(const std::string& imagePath,
    const std::vector<unsigned char>& header,
    const std::vector<unsigned char>& rgbData) {
    FILE* file = fopen(imagePath.c_str(), "wb");
    if (!file) {
      std::cerr << "Error: Could not open file for writing: " << imagePath
        << std::endl;
      return false;
    }

    if (fwrite(header.data(), 1, header.size(), file) != header.size()) {
      std::cerr << "Error: Failed to write BMP header to file." << std::endl;
      fclose(file);
      return false;
    }

    if (fwrite(rgbData.data(), 1, rgbData.size(), file) != rgbData.size()) {
      std::cerr << "Error: Failed to write BMP image data to file." << std::endl;
      fclose(file);
      return false;
    }

    fclose(file);
    return true;
  }

  void us_recon_core_export GetImageDimensions(const unsigned char* header,
    unsigned int& width,
    unsigned int& height) {
    width = *reinterpret_cast<const unsigned int*>(&header[WIDTH_INDEX]);
    height = *reinterpret_cast<const unsigned int*>(&header[HEIGHT_INDEX]);
  }
}