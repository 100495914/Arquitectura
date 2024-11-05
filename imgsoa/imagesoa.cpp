//
// Created by diego on 20/10/24.
//
#include "imagesoa.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cmath>

void scaleIntensity(ImageSOA_8bit& pixels, int currentMax, int newMax) {
  // Usar tres bucles mejor para optimizacion de cache, (probablemente)
  for (auto& redPixel : pixels.red) {
    redPixel = static_cast<uint8_t>(std::round(redPixel * static_cast<double>(newMax) / currentMax));
  }
  for (auto& greenPixel : pixels.green) {
    greenPixel = static_cast<uint8_t>(std::round(greenPixel * static_cast<double>(newMax) / currentMax));
  }
  for (auto& bluePixel : pixels.blue) {
    bluePixel = static_cast<uint8_t>(std::round(bluePixel * static_cast<double>(newMax) / currentMax));
  }
}

void scaleIntensity(ImageSOA_16bit& pixels, int currentMax, int newMax) {
  for (auto& redPixel : pixels.red) {
    redPixel = static_cast<uint16_t>(std::round(redPixel * static_cast<double>(newMax) / currentMax));
  }
  for (auto& greenPixel : pixels.green) {
    greenPixel = static_cast<uint16_t>(std::round(greenPixel * static_cast<double>(newMax) / currentMax));
  }
  for (auto& bluePixel : pixels.blue) {
    bluePixel = static_cast<uint16_t>(std::round(bluePixel * static_cast<double>(newMax) / currentMax));
  }
}

ImageSOA_8bit loadImage(const std::string& filename, int width, int height, int maxColorValue) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file " + filename);
  }
  // TODO
};
