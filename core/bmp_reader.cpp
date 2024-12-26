#include "bmp_reader.h"
#include <algorithm>
#include <iostream>
#include <vector>

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

  unsigned int headerSize = *reinterpret_cast<unsigned int*>(&header[HEADER_SIZE_INDEX]);
  unsigned int imageSize = *reinterpret_cast<unsigned int*>(&header[IMAGE_SIZE_INDEX]);
  unsigned int width = *reinterpret_cast<unsigned int*>(&header[WIDTH_INDEX]);
  unsigned int height = *reinterpret_cast<unsigned int*>(&header[HEIGHT_INDEX]);
  unsigned short bitsPerPixel =
      *reinterpret_cast<unsigned short*>(&header[BITS_PER_PIXEL_INDEX]);

  if (headerSize > HEADER_SIZE) {
    header.resize(headerSize);
    rewind(file);
    if (fread(header.data(), 1, headerSize, file) != headerSize) {
      std::cerr << "Error: Failed to read the extended BMP header" << std::endl;
      fclose(file);
      return false;
    }
  }

  if (headerSize == 0) {
    headerSize = HEADER_SIZE;
  }

  if (imageSize == 0) {
    imageSize = width * height * (bitsPerPixel / BITS_PER_BYTE);
  }

  rgbData.resize(imageSize);
  if (fread(rgbData.data(), 1, imageSize, file) != imageSize) {
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


bool ApplyGrayFilter(const std::vector<unsigned char>& header,
                     std::vector<unsigned char>& rgbData) {
  const size_t imageSize =
      *reinterpret_cast<const unsigned int*>(&header[IMAGE_SIZE_INDEX]);
  const short bytesPerPixel = (*reinterpret_cast<const unsigned short*>(
                                  &header[BITS_PER_PIXEL_INDEX])) /
                              BITS_PER_BYTE;

  for (size_t i = 0; i < imageSize; i += bytesPerPixel) {
    unsigned char gray = static_cast<unsigned char>(
        0.3 * rgbData[i + 2] + 0.59 * rgbData[i + 1] + 0.11 * rgbData[i]);
    rgbData[i] = gray;      // R
    rgbData[i + 1] = gray;  // G
    rgbData[i + 2] = gray;  // B
  }
  return true;
}

bool FlipVertically(const std::vector<unsigned char>& header,
                    std::vector<unsigned char>& rgbData) {
  const unsigned int width =
      *reinterpret_cast<const unsigned int*>(&header[WIDTH_INDEX]);
  const unsigned int height =
      *reinterpret_cast<const unsigned int*>(&header[HEIGHT_INDEX]);
  const unsigned short bytesPerPixel =
      (*reinterpret_cast<const unsigned short*>(
          &header[BITS_PER_PIXEL_INDEX])) /
      BITS_PER_BYTE;


  const size_t rowSize = width * bytesPerPixel;
  std::vector<unsigned char> tempRow(rowSize);

  for (unsigned int row = 0; row < height / 2; ++row) {
    unsigned char* topRow = rgbData.data() + row * rowSize;
    unsigned char* bottomRow = rgbData.data() + (height - row - 1) * rowSize;

    std::memcpy(tempRow.data(), topRow, rowSize);
    std::memcpy(topRow, bottomRow, rowSize);
    std::memcpy(bottomRow, tempRow.data(), rowSize);
  }

  return true;
}

void us_recon_core_export GetImageDimensions(const unsigned char* header,
                                             unsigned int& width,
                                             unsigned int& height) {
  width = *reinterpret_cast<const unsigned int*>(&header[WIDTH_INDEX]);
  height = *reinterpret_cast<const unsigned int*>(&header[HEIGHT_INDEX]);
}
