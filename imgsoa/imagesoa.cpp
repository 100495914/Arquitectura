//
// Created by diego on 20/10/24.
//
#include "imagesoa.hpp"

#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>

void scaleIntensity(ImageSOA_8bit & pixels, int currentMax, int newMax) {
  // Usar tres bucles mejor para optimizacion de cache, (probablemente)
  for (auto & redPixel : pixels.red) {
    redPixel =
        static_cast<uint8_t>(std::round(redPixel * static_cast<double>(newMax) / currentMax));
  }
  for (auto & greenPixel : pixels.green) {
    greenPixel =
        static_cast<uint8_t>(std::round(greenPixel * static_cast<double>(newMax) / currentMax));
  }
  for (auto & bluePixel : pixels.blue) {
    bluePixel =
        static_cast<uint8_t>(std::round(bluePixel * static_cast<double>(newMax) / currentMax));
  }
}

void scaleIntensity(ImageSOA_16bit & pixels, int currentMax, int newMax) {
  for (auto & redPixel : pixels.red) {
    redPixel =
        static_cast<uint16_t>(std::round(redPixel * static_cast<double>(newMax) / currentMax));
  }
  for (auto & greenPixel : pixels.green) {
    greenPixel =
        static_cast<uint16_t>(std::round(greenPixel * static_cast<double>(newMax) / currentMax));
  }
  for (auto & bluePixel : pixels.blue) {
    bluePixel =
        static_cast<uint16_t>(std::round(bluePixel * static_cast<double>(newMax) / currentMax));
  }
}

namespace {
  // Helper function to load pixel data into the ImageSOA_8bit struct
  void hlpr_loadPixelData(std::ifstream & file, size_t const totalPixels, ImageSOA_8bit & image) {
    for (size_t i = 0; i < totalPixels; ++i) {
      uint8_t red   = 0;
      uint8_t green = 0;
      uint8_t blue  = 0;
      file.read(reinterpret_cast<char *>(&red), sizeof(red));
      file.read(reinterpret_cast<char *>(&green), sizeof(green));
      file.read(reinterpret_cast<char *>(&blue), sizeof(blue));

      image.red[i]   = red;
      image.green[i] = green;
      image.blue[i]  = blue;
    }
  }

  // Helper function to load pixel data into the ImageSOA_16bit struct
  void hlpr_loadPixelData(std::ifstream & file, size_t const totalPixels, ImageSOA_16bit & image) {
    for (size_t i = 0; i < totalPixels; ++i) {
      uint16_t red   = 0;
      uint16_t green = 0;
      uint16_t blue  = 0;
      file.read(reinterpret_cast<char *>(&red), sizeof(red));
      file.read(reinterpret_cast<char *>(&green), sizeof(green));
      file.read(reinterpret_cast<char *>(&blue), sizeof(blue));

      image.red[i]   = red;
      image.green[i] = green;
      image.blue[i]  = blue;
    }
  }

  // Helper function to load image data (open file, skip header, read pixels) for 8-bit images
  ImageSOA_8bit hlpr_loadImageData_8bit(std::string const & filename, size_t totalPixels) {
    ImageSOA_8bit image(totalPixels);

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) { throw std::ios_base::failure("Failed to open the file."); }

    hlpr_loadPixelData(file, totalPixels, image);
    file.close();
    return image;
  }

  // Helper function to load image data (open file, skip header, read pixels) for 16-bit images
  ImageSOA_16bit hlpr_loadImageData_16bit(std::string const & filename, size_t totalPixels) {
    ImageSOA_16bit image(totalPixels);

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) { throw std::ios_base::failure("Failed to open the file."); }

    hlpr_loadPixelData(file, totalPixels, image);
    file.close();
    return image;
  }
}  // namespace

std::variant<ImageSOA_8bit, ImageSOA_16bit> loadImage(std::string const & filename,
                                                      PPMMetadata const & metadata) {
  size_t const totalPixels =
      static_cast<size_t>(metadata.width) * static_cast<size_t>(metadata.height);

  if (metadata.maxColorValue <= MAX_8BIT_VALUE) {
    return hlpr_loadImageData_8bit(filename, totalPixels);
  }
  if (metadata.maxColorValue <= MAX_16BIT_VALUE) {
    return hlpr_loadImageData_16bit(filename, totalPixels);
  }
  throw std::invalid_argument("Invalid maxColorValue for 8-bit image. Must be <= 255.");
}

PPMMetadata getPPMMetadata(const std::string& filename) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: " + filename);
  }

  PPMMetadata metadata;
  file >> metadata.magicNumber;
  file >> metadata.width >> metadata.height >> metadata.maxColorValue;
  file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  file.close();

  return metadata;
}