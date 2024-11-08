//
// Created by diego on 20/10/24.
//
#include "imagesoa.hpp"

#include <cmath>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <stdexcept>

// Resize the image (resize all color channels)
void ImageSOA_8bit::resize(uint width, uint height) {
  auto newSize = static_cast<size_t>(width * height);
  // TODO
}

// Scale intensity for each channel (for 8-bit values)
void ImageSOA_8bit::scaleIntensity(uint currentMax, uint newMax) {
  auto const scale = static_cast<float>(newMax) / static_cast<float>(currentMax);

  for (size_t i = 0; i < red.size(); ++i) {
    red[i]   = std::min(static_cast<uint8_t>(static_cast<float>(red[i]) * scale), static_cast<uint8_t>(newMax));
    green[i] = std::min(static_cast<uint8_t>(static_cast<float>(green[i]) * scale), static_cast<uint8_t>(newMax));
    blue[i]  = std::min(static_cast<uint8_t>(static_cast<float>(blue[i]) * scale), static_cast<uint8_t>(newMax));
  }
}

// Remove the least frequent colors from the image
void ImageSOA_8bit::removeLeastFrequentColors(int n) {
  std::cout << "Removing " << n << " least frequent colors from 8-bit image." << '\n';
  // TODO
}

// Compress the image (example function, adjust as needed)
void ImageSOA_8bit::compress() {
  std::cout << "Compressing 8-bit image." << '\n';
  // TODO
}

// Resize the image (resize all color channels)
void ImageSOA_16bit::resize(uint width, uint height) {
  auto newSize = static_cast<size_t>(width * height);
  // TODO
}

// Scale intensity for each channel (for 16-bit values)
void ImageSOA_16bit::scaleIntensity(uint currentMax, uint newMax) {
  auto const scale = static_cast<float>(newMax) / static_cast<float>(currentMax);

  for (size_t i = 0; i < red.size(); ++i) {
    red[i]   = std::min(static_cast<uint16_t>(static_cast<float>(red[i]) * scale), static_cast<uint16_t>(newMax));
    green[i] = std::min(static_cast<uint16_t>(static_cast<float>(green[i]) * scale), static_cast<uint16_t>(newMax));
    blue[i]  = std::min(static_cast<uint16_t>(static_cast<float>(blue[i]) * scale), static_cast<uint16_t>(newMax));
  }
}

// Remove the least frequent colors from the image
void ImageSOA_16bit::removeLeastFrequentColors(int n) {
  std::cout << "Removing " << n << " least frequent colors from 16-bit image." << '\n';
  // TODO
}

// Compress the image (example function, adjust as needed)
void ImageSOA_16bit::compress() {
  std::cout << "Compressing 16-bit image." << '\n';
  // TODO
}
