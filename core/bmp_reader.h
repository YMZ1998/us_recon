#pragma once
#include <string>
#include "us_recon_export.h"

// This header location indices are specified on the bitmap format specification (http://www.fastgraph.com/help/bmp_header_format.html)
constexpr unsigned short HEADER_SIZE_INDEX = 10;
constexpr unsigned short IMAGE_SIZE_INDEX = 34;
constexpr unsigned short WIDTH_INDEX = 18;
constexpr unsigned short HEIGHT_INDEX = 22;
constexpr unsigned short BITS_PER_PIXEL_INDEX = 28;

constexpr unsigned short BITS_PER_BYTE = 8;
constexpr unsigned short HEADER_SIZE = 54;

bool us_recon_core_export ReadBMP(std::string imagepath, unsigned char*& header,
             unsigned char*& rgbData, unsigned int& headerSize,
             unsigned int& imageSize);
bool us_recon_core_export WriteBMP(std::string imagepath, unsigned char*& header,
              unsigned char*& rgbData, size_t headerSize, size_t imageSize);
bool us_recon_core_export ApplyGrayFilter(unsigned char*& header, unsigned char*& rgbData);
bool us_recon_core_export FlipVertically(unsigned char*& header, unsigned char*& rgbData);