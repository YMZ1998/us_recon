/*
 *    BMP Loader written by Raydelto Hernandez  (raydelto@yahoo.com)
 */

#include <cstdio>
#include <string>
#include <iostream>
#include "bmp_reader.h"
using namespace std;

int main(void)
{
  unsigned char* rgbData;
  unsigned char* header;
  unsigned int imageSize;
  unsigned int headerSize;
  cout << "Reading the BMP file ... " << endl;
  ReadBMP("D:\\Code\\us_recon\\data\\PATI000\\01.BMP", header, rgbData, headerSize, imageSize);
  ApplyGrayFilter(header, rgbData);
  FlipVertically(header, rgbData);

  cout << "Writing a new BMP file based on data read from a BMP in the previous step ..." << endl;
  WriteBMP("D:\\Code\\us_recon\\data\\01-out.BMP", header, rgbData, headerSize, imageSize);
  cout << "Freeing resources..." << endl;
  delete[] rgbData;
  delete[] header;
  cout << "This application has ended its execution." << endl;
  return 0;
}