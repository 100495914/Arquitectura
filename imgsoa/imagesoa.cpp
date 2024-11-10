//
// Created by diego on 20/10/24.
//
#include "imagesoa.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <variant>

std::unique_ptr<ImageSOA> loadImage(std::string const & filepath) {
  std::cerr << "Trying to open: " << filepath << '\n';
  std::ifstream file(filepath, std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "Error opening file!" << '\n';
    return nullptr;
  }

  PPMMetadata metadata;
  file >> metadata.magicNumber >> metadata.width >> metadata.height >> metadata.maxColorValue;
  file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  // Create the appropriate image object based on maxColorValue
  std::unique_ptr<ImageSOA> image;
  if (metadata.maxColorValue <= MAX_8BIT_VALUE) {
    // 8-bit image
    image = std::make_unique<ImageSOA_8bit>(metadata);
  } else if (metadata.maxColorValue <= MAX_16BIT_VALUE) {
    // 16-bit image
    image = std::make_unique<ImageSOA_16bit>(metadata);
  } else {
    std::cerr << "Unsupported max color value!" << '\n';
    return nullptr;
  }

  // Now load the pixel data
  image->loadData(file);  // Calls either ImageSOA_8bit::loadData or ImageSOA_16bit::loadData based
                          // on the object type

  return image;
}

void ImageSOA_8bit::loadData(std::ifstream & file) {
  if (!file.is_open()) { throw std::ios_base::failure("Failed to open the file."); }

  uint8_t red   = 0;
  uint8_t green = 0;
  uint8_t blue  = 0;

  // Read pixel data (each pixel consists of 3 bytes: R, G, B)
  size_t const size = static_cast<size_t>(gWidth()) * gHeight();

  for (size_t i = 0; i < size; ++i) {
    file.read(reinterpret_cast<char *>(&red), sizeof(red));
    file.read(reinterpret_cast<char *>(&green), sizeof(green));
    file.read(reinterpret_cast<char *>(&blue), sizeof(blue));

    if (!file) { throw std::ios_base::failure("Failed to read pixel data."); }

    // Assign the values to the respective channels
    gRed()[i]   = red;
    gGreen()[i] = green;
    gBlue()[i]  = blue;
  }

  // Check if file reading was successful
  if (file.fail()) { throw std::ios_base::failure("File reading error occurred."); }
}

void ImageSOA_8bit::saveToFile(std::string const & filename) {
  std::ofstream file(filename, std::ios::out | std::ios::binary);

  // Check if the file is opened successfully
  if (!file.is_open()) { throw std::ios_base::failure("Failed to open file for saving."); }

  // Write the PPM header for P6 format
  file << "P6\n";
  file << gWidth() << " " << gHeight() << "\n";
  file << static_cast<int>(gMaxColorValue()) << "\n";
  // Write pixel data (each pixel consists of 3 bytes: R, G, B)
  size_t const size = static_cast<size_t>(gWidth()) * gHeight();
  for (size_t i = 0; i < size; ++i) {
    uint8_t red   = gRed()[i];
    uint8_t green = gGreen()[i];
    uint8_t blue  = gBlue()[i];
    file.write(reinterpret_cast<char const *>(&red), sizeof(red));
    file.write(reinterpret_cast<char const *>(&green), sizeof(green));
    file.write(reinterpret_cast<char const *>(&blue), sizeof(blue));
  }

  file.close();
}

// Scale intensity for each channel (for 8-bit values)
void ImageSOA_8bit::scaleIntensity(uint const newMax) {
  if (newMax > MAX_8BIT_VALUE) {
    throw std::invalid_argument(
        "newMax surpasses the 8-bit int limit, use the other scaling function");
  }
  auto const scale = static_cast<float>(newMax) / static_cast<float>(gMaxColorValue());
  for (uint8_t & subpixel : red) {
    subpixel = static_cast<uint8_t>(std::floor(static_cast<float>(subpixel) * scale));
  }
  for (uint8_t & subpixel : green) {
    subpixel = static_cast<uint8_t>(std::floor(static_cast<float>(subpixel) * scale));
  }
  for (uint8_t & subpixel : blue) {
    subpixel = static_cast<uint8_t>(std::floor(static_cast<float>(subpixel) * scale));
  }
}

std::unique_ptr<ImageSOA_16bit>
    ImageSOA_8bit::scaleIntensityChannelSizeChange(uint const newMax) const {
  if (newMax > MAX_16BIT_VALUE) {
    throw std::invalid_argument("newMax exceeds the 16-bit integer range");
  }
  if (newMax <= MAX_8BIT_VALUE) {
    throw std::invalid_argument("newMax is within 8-bit range; this function is not necessary");
  }

  // Create new metadata for the scaled image
  PPMMetadata metadata;
  metadata.maxColorValue = newMax;
  metadata.magicNumber   = gMagicNumber();
  metadata.width         = gWidth();
  metadata.height        = gHeight();

  // Create a new 16-bit image with the updated metadata
  auto image = std::make_unique<ImageSOA_16bit>(metadata);

  // Calculate scaling factor
  auto const scale = static_cast<float>(newMax) / static_cast<float>(gMaxColorValue());

  for (uint16_t & subpixel : image->gRed()) {
    subpixel = static_cast<uint16_t>(std::floor(static_cast<float>(subpixel) * scale));
  }
  for (uint16_t & subpixel : image->gGreen()) {
    subpixel = static_cast<uint16_t>(std::floor(static_cast<float>(subpixel) * scale));
  }
  for (uint16_t & subpixel : image->gBlue()) {
    subpixel = static_cast<uint16_t>(std::floor(static_cast<float>(subpixel) * scale));
  }

  return image;
}

void ImageSOA_16bit::loadData(std::ifstream & file) {
  if (!file.is_open()) { throw std::ios_base::failure("Failed to open the file."); }

  // Skip width, height, and max color value
  size_t width         = 0;
  size_t height        = 0;
  size_t maxColorValue = 0;
  file >> width >> height >> maxColorValue;

  // Ensure that the image dimensions match
  if (width != gWidth() || height != gHeight()) {
    throw std::ios_base::failure("Image dimensions do not match expected size.");
  }

  // Skip any remaining newline after header
  file.ignore(1, '\n');
  uint16_t red   = 0;
  uint16_t green = 0;
  uint16_t blue  = 0;
  // Read pixel data (each pixel consists of 6 bytes: R, G, B, each 2 bytes)
  size_t const size = static_cast<size_t>(gWidth()) * gHeight();
  for (size_t i = 0; i < size; ++i) {
    // Read each channel as 2 bytes
    file.read(reinterpret_cast<char *>(&red), sizeof(red));
    file.read(reinterpret_cast<char *>(&green), sizeof(green));
    file.read(reinterpret_cast<char *>(&blue), sizeof(blue));

    if (!file) { throw std::ios_base::failure("Failed to read pixel data."); }

    // Assign the values to the respective channels
    gRed()[i]   = red;
    gGreen()[i] = green;
    gBlue()[i]  = blue;
  }

  // Check if file reading was successful
  if (file.fail()) { throw std::ios_base::failure("File reading error occurred."); }
}

void ImageSOA_16bit::saveToFile(std::string const & filename) {
  std::ofstream file(filename, std::ios::out | std::ios::binary);

  // Check if the file is opened successfully
  if (!file.is_open()) { throw std::ios_base::failure("Failed to open file for saving."); }

  // Write the PPM header for P6 format
  file << "P6\n";
  file << gWidth() << " " << gHeight() << "\n";
  file << static_cast<int>(gMaxColorValue()) << "\n";

  // Write pixel data (each pixel consists of 6 bytes: R, G, B, each 2 bytes)
  size_t const size = static_cast<size_t>(gWidth()) * gHeight();
  for (size_t i = 0; i < size; ++i) {
    uint16_t red   = gRed()[i];
    uint16_t green = gGreen()[i];
    uint16_t blue  = gBlue()[i];
    file.write(reinterpret_cast<char const *>(&red), sizeof(red));
    file.write(reinterpret_cast<char const *>(&green), sizeof(green));
    file.write(reinterpret_cast<char const *>(&blue), sizeof(blue));
  }

  file.close();
}

// Scale intensity for each channel (for 16-bit values)
void ImageSOA_16bit::scaleIntensity(uint const newMax) {
  if (newMax > MAX_16BIT_VALUE) {
    throw std::invalid_argument("newMax surpasses the 16 bit int limit");
  }
  if (MAX_8BIT_VALUE >= newMax) {
    throw std::invalid_argument("newMax is inside the 8 bit int limit, convert to 8 bit image");
  }
  auto const scale = static_cast<float>(newMax) / static_cast<float>(gMaxColorValue());
  for (uint16_t & subpixel : red) {
    subpixel = static_cast<uint16_t>(std::floor(static_cast<float>(subpixel) * scale));
  }
  for (uint16_t & subpixel : green) {
    subpixel = static_cast<uint16_t>(std::floor(static_cast<float>(subpixel) * scale));
  }
  for (uint16_t & subpixel : blue) {
    subpixel = static_cast<uint16_t>(std::floor(static_cast<float>(subpixel) * scale));
  }
}

std::unique_ptr<ImageSOA_8bit>
    ImageSOA_16bit::scaleIntensityChannelSizeChange(uint const newMax) const {
  if (newMax > MAX_16BIT_VALUE) {
    throw std::invalid_argument("newMax exceeds the 16-bit integer range");
  }
  if (newMax <= MAX_8BIT_VALUE) {
    throw std::invalid_argument("newMax is within 8-bit range; this function is not necessary");
  }

  // Create new metadata for the scaled image
  PPMMetadata metadata;
  metadata.maxColorValue = newMax;
  metadata.magicNumber   = gMagicNumber();
  metadata.width         = gWidth();
  metadata.height        = gHeight();

  // Create a new 16-bit image with the updated metadata
  auto image = std::make_unique<ImageSOA_8bit>(metadata);

  // Calculate scaling factor
  auto const scale = static_cast<float>(newMax) / static_cast<float>(gMaxColorValue());

  for (uint8_t & subpixel : image->gRed()) {
    subpixel = static_cast<uint8_t>(std::floor(static_cast<float>(subpixel) * scale));
  }
  for (uint8_t & subpixel : image->gGreen()) {
    subpixel = static_cast<uint8_t>(std::floor(static_cast<float>(subpixel) * scale));
  }
  for (uint8_t & subpixel : image->gBlue()) {
    subpixel = static_cast<uint8_t>(std::floor(static_cast<float>(subpixel) * scale));
  }

  return image;
}

int main() {
  try {
    std::string const input  = "../input/sabatini.ppm";
    std::string const output = "../output/sabatini.ppm";
    // Load an image from file (assumed to be in PPM P6 format with 16-bit channels)
    auto const image = loadImage(input);
    // Optionally, perform image processing here (scaling, manipulation, etc.)
    image->scaleIntensityChannelSizeChange(10000);

  } catch (std::exception const & e) { std::cerr << "Error: " << e.what() << '\n'; }

  return 0;
}
