//
// Created by diego on 20/10/24.
//
// imagesoa.cpp
#include "imageaos.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
// imageaos.cpp
#include "imageaos.hpp"
#include <cmath>

void scaleIntensity(std::vector<Pixel>& pixels, int currentMax, int newMax) {
  for (auto& pixel : pixels) {
    pixel.red = static_cast<int>(std::round(pixel.red * newMax / static_cast<double>(currentMax)));
    pixel.green = static_cast<int>(std::round(pixel.green * newMax / static_cast<double>(currentMax)));
    pixel.blue = static_cast<int>(std::round(pixel.blue * newMax / static_cast<double>(currentMax)));
  }
}

void saveImage(const std::string& filename, const std::vector<Pixel>& pixels, int width, int height, int maxColorValue) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    // Escribir el encabezado PPM
    file << "P6\n" << width << " " << height << "\n" << maxColorValue << "\n";

    int bytesPerChannel = (maxColorValue < 256) ? 1 : 2;

    for (const auto& pixel : pixels) {
        if (bytesPerChannel == 1) {
            // Escribir 1 byte por cada canal de color
            unsigned char r = static_cast<unsigned char>(pixel.red);
            unsigned char g = static_cast<unsigned char>(pixel.green);
            unsigned char b = static_cast<unsigned char>(pixel.blue);
            file.write(reinterpret_cast<char*>(&r), 1);
            file.write(reinterpret_cast<char*>(&g), 1);
            file.write(reinterpret_cast<char*>(&b), 1);
        } else {
            // Escribir 2 bytes por cada canal de color (little-endian)
            unsigned short r = static_cast<unsigned short>(pixel.red);
            unsigned short g = static_cast<unsigned short>(pixel.green);
            unsigned short b = static_cast<unsigned short>(pixel.blue);
            file.write(reinterpret_cast<char*>(&r), 2);
            file.write(reinterpret_cast<char*>(&g), 2);
            file.write(reinterpret_cast<char*>(&b), 2);
        }
    }

    file.close();
}

std::vector<Pixel> loadImage(const std::string& filename, int width, int height, int maxColorValue) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: " + filename);
  }

  // Saltar el encabezado PPM (Número mágico, ancho, alto, y valor máximo de color)
  std::string magicNumber;
  file >> magicNumber;
  file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  // Leer píxeles
  std::vector<Pixel> pixels(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
  int bytesPerChannel = (maxColorValue < 256) ? 1 : 2;  // 1 byte si maxColorValue < 256, 2 bytes si >= 256

  for (auto& pixel : pixels) {
    if (bytesPerChannel == 1) {
      // Leer 1 byte por cada canal de color
      unsigned char r, g, b;
      file.read(reinterpret_cast<char*>(&r), 1);
      file.read(reinterpret_cast<char*>(&g), 1);
      file.read(reinterpret_cast<char*>(&b), 1);
      pixel.red = r;
      pixel.green = g;
      pixel.blue = b;
    } else {
      // Leer 2 bytes por cada canal de color (little-endian)
      unsigned short r, g, b;
      file.read(reinterpret_cast<char*>(&r), 2);
      file.read(reinterpret_cast<char*>(&g), 2);
      file.read(reinterpret_cast<char*>(&b), 2);
      pixel.red = r;
      pixel.green = g;
      pixel.blue = b;
    }
  }

  file.close();
  return pixels;
}


PPMMetadata getPPMMetadata(const std::string& filename) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: " + filename);
  }

  PPMMetadata metadata;

  file >> metadata.magicNumber;
  if (metadata.magicNumber != "P6") {
    throw std::runtime_error("Invalid PPM format: incorrect magic number");
  }

  file >> metadata.width >> metadata.height >> metadata.maxColorValue;

  if (metadata.width <= 0 || metadata.height <= 0) {
    throw std::runtime_error("Invalid PPM format: width and height must be positive");
  }
  if (metadata.maxColorValue <= 0 || metadata.maxColorValue > 65535) {
    throw std::runtime_error("Invalid PPM format: max color value must be between 1 and 65535");
  }

  file.ignore(1);
  file.close();

  return metadata;
}
