//
// Created by diego on 20/10/24.
//
#include "imagesoa.hpp"

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

PPMMetadata loadMetadata(std::string const & filepath) {
  std::cerr << "Trying to open: " << filepath << '\n';
  std::ifstream file(filepath, std::ios::binary);
  if (!file.is_open()) { throw std::runtime_error("Failed to open file"); }

  PPMMetadata metadata;
  file >> metadata.magicNumber >> metadata.width >> metadata.height >> metadata.maxColorValue;
  file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  // Create the appropriate image object based on maxColorValue
  return metadata;
}

void ImageSOA_8bit::loadData(std::string const & filepath) {
  std::ifstream file(filepath, std::ios::binary);
  file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  if (!file.is_open()) { throw std::ios_base::failure("Failed to open the file."); }
  size_t width         = 0;
  size_t height        = 0;
  size_t maxColorValue = 0;
  file >> width >> height >> maxColorValue;
  if (width != gWidth() || height != gHeight()) {
    throw std::ios_base::failure("Image dimensions do not match expected size.");
  }
  file.ignore(1, '\n');

  uint8_t red   = 0;
  uint8_t green = 0;
  uint8_t blue  = 0;

  // Read pixel data (each pixel consists of 3 bytes: R, G, B)
  size_t const size = static_cast<size_t>(gWidth()) * gHeight();

  for (size_t i = 0; i < size; ++i) {
    file.read(reinterpret_cast<char *>(&red),  // NOLINT(*-pro-type-reinterpret-cast)
              sizeof(red));
    file.read(reinterpret_cast<char *>(&green),  // NOLINT(*-pro-type-reinterpret-cast)
              sizeof(green));
    file.read(reinterpret_cast<char *>(&blue),  // NOLINT(*-pro-type-reinterpret-cast)
              sizeof(blue));

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
    file.write(reinterpret_cast<char const *>(&red),  // NOLINT(*-pro-type-reinterpret-cast)
               sizeof(red));
    file.write(reinterpret_cast<char const *>(&green),  // NOLINT(*-pro-type-reinterpret-cast)
               sizeof(green));
    file.write(reinterpret_cast<char const *>(&blue),  // NOLINT(*-pro-type-reinterpret-cast)
               sizeof(blue));
  }

  file.close();
}

// Scale intensity for each channel (for 8-bit values)
void ImageSOA_8bit::maxLevel(uint const newMax) {
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
  sMaxColorValue(newMax);
}

std::unique_ptr<ImageSOA_16bit> ImageSOA_8bit::maxLevelChangeChannelSize(uint const newMax) {
  if (newMax > MAX_16BIT_VALUE) {
    throw std::invalid_argument("newMax exceeds the 16-bit integer range");
  }
  if (newMax <= MAX_8BIT_VALUE) {
    throw std::invalid_argument("newMax is within 8-bit range; this function is not necessary");
  }
  PPMMetadata metadata;
  metadata.maxColorValue = newMax;
  metadata.magicNumber   = gMagicNumber();
  metadata.width         = gWidth();
  metadata.height        = gHeight();

  auto image = std::make_unique<ImageSOA_16bit>(metadata);

  auto const scale = static_cast<float>(newMax) / static_cast<float>(gMaxColorValue());

  auto const & originalRed = gRed();
  auto & newRed            = image->gRed();
  for (size_t i = 0; i < originalRed.size(); ++i) {
    newRed[i] = static_cast<uint16_t>(std::floor(static_cast<float>(originalRed[i]) * scale));
  }

  auto const & originalGreen = gGreen();
  auto & newGreen            = image->gGreen();
  for (size_t i = 0; i < originalGreen.size(); ++i) {
    newGreen[i] = static_cast<uint16_t>(std::floor(static_cast<float>(originalGreen[i]) * scale));
  }

  auto const & originalBlue = gBlue();
  auto & newBlue            = image->gBlue();
  for (size_t i = 0; i < originalBlue.size(); ++i) {
    newBlue[i] = static_cast<uint16_t>(std::floor(static_cast<float>(originalBlue[i]) * scale));
  }
  return image;
}

void ImageSOA_16bit::loadData(std::string const & filepath) {
  std::ifstream file(filepath, std::ios::binary);
  file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  if (!file.is_open()) { throw std::ios_base::failure("Failed to open the file."); }
  size_t width         = 0;
  size_t height        = 0;
  size_t maxColorValue = 0;
  file >> width >> height >> maxColorValue;
  if (width != gWidth() || height != gHeight()) {
    throw std::ios_base::failure("Image dimensions do not match expected size.");
  }
  file.ignore(1, '\n');
  uint16_t red      = 0;
  uint16_t green    = 0;
  uint16_t blue     = 0;
  size_t const size = static_cast<size_t>(gWidth()) * gHeight();
  for (size_t i = 0; i < size; ++i) {
    file.read(reinterpret_cast<char *>(&red),  // NOLINT(*-pro-type-reinterpret-cast)
              2);
    file.read(reinterpret_cast<char *>(&green),  // NOLINT(*-pro-type-reinterpret-cast)
              2);
    file.read(reinterpret_cast<char *>(&blue),  // NOLINT(*-pro-type-reinterpret-cast)
              2);
    if (!file) { throw std::ios_base::failure("Failed to read pixel data."); }
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
  uint16_t const hex_ff      = 0xFF;
  uint16_t const shift_value = 8;
  size_t const size          = static_cast<size_t>(gWidth()) * gHeight();
  for (size_t i = 0; i < size; ++i) {
    uint16_t const red   = gRed()[i];
    uint16_t const green = gGreen()[i];
    uint16_t const blue  = gBlue()[i];

    // Use std::array instead of C-style array
    std::array<char, 2> redBytes   = {static_cast<char>(red & hex_ff),
                                      static_cast<char>((red >> shift_value) & hex_ff)};
    std::array<char, 2> greenBytes = {static_cast<char>(green & hex_ff),
                                      static_cast<char>((green >> shift_value) & hex_ff)};
    std::array<char, 2> blueBytes  = {static_cast<char>(blue & hex_ff),
                                      static_cast<char>((blue >> shift_value) & hex_ff)};

    // Write the bytes to the file
    file.write(redBytes.data(), 2);
    file.write(greenBytes.data(), 2);
    file.write(blueBytes.data(), 2);
  }

  file.close();
}

void ImageSOA_16bit::saveToFileBE(std::string const & filename) {
  std::ofstream file(filename, std::ios::out | std::ios::binary);

  // Check if the file is opened successfully
  if (!file.is_open()) { throw std::ios_base::failure("Failed to open file for saving."); }

  // Write the PPM header for P6 format
  file << "P6\n";
  file << gWidth() << " " << gHeight() << "\n";
  file << static_cast<int>(gMaxColorValue()) << "\n";

  // Write pixel data (each pixel consists of 6 bytes: R, G, B, each 2 bytes)
  uint16_t const hex_ff      = 0xFF;
  uint16_t const shift_value = 8;
  size_t const size          = static_cast<size_t>(gWidth()) * gHeight();

  for (size_t i = 0; i < size; ++i) {
    uint16_t const red   = gRed()[i];
    uint16_t const green = gGreen()[i];
    uint16_t const blue  = gBlue()[i];

    // Invert the byte order for each channel
    std::array<char, 2> redBytes   = {static_cast<char>((red >> shift_value) & hex_ff),
                                      static_cast<char>(red & hex_ff)};
    std::array<char, 2> greenBytes = {static_cast<char>((green >> shift_value) & hex_ff),
                                      static_cast<char>(green & hex_ff)};
    std::array<char, 2> blueBytes  = {static_cast<char>((blue >> shift_value) & hex_ff),
                                      static_cast<char>(blue & hex_ff)};

    // Write the inverted bytes to the file
    file.write(redBytes.data(), 2);
    file.write(greenBytes.data(), 2);
    file.write(blueBytes.data(), 2);
  }

  file.close();
}

// Scale intensity for each channel (for 16-bit values)
void ImageSOA_16bit::maxLevel(uint const newMax) {
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
  sMaxColorValue(newMax);
}

std::unique_ptr<ImageSOA_8bit> ImageSOA_16bit::maxLevelChangeChannelSize(uint const newMax) {
  if (newMax > MAX_16BIT_VALUE) {
    throw std::invalid_argument("newMax exceeds the 16-bit integer range");
  }
  if (newMax > MAX_8BIT_VALUE) {
    throw std::invalid_argument("newMax is within 16-bit range; this function is not necessary");
  }

  // Create new metadata for the scaled image
  PPMMetadata metadata;
  metadata.maxColorValue = newMax;
  metadata.magicNumber   = gMagicNumber();
  metadata.width         = gWidth();
  metadata.height        = gHeight();

  // Create a new 16-bit image with the updated metadata
  auto image = std::make_unique<ImageSOA_8bit>(metadata);

  auto const scale = static_cast<float>(newMax) / static_cast<float>(gMaxColorValue());

  auto const & originalRed = gRed();
  auto & newRed            = image->gRed();
  for (size_t i = 0; i < originalRed.size(); ++i) {
    newRed[i] = static_cast<uint8_t>(std::floor(static_cast<float>(originalRed[i]) * scale));
  }

  auto const & originalGreen = gGreen();
  auto & newGreen            = image->gGreen();
  for (size_t i = 0; i < originalGreen.size(); ++i) {
    newGreen[i] = static_cast<uint8_t>(std::floor(static_cast<float>(originalGreen[i]) * scale));
  }

  auto const & originalBlue = gBlue();
  auto & newBlue            = image->gBlue();
  for (size_t i = 0; i < originalBlue.size(); ++i) {
    newBlue[i] = static_cast<uint8_t>(std::floor(static_cast<float>(originalBlue[i]) * scale));
  }
  return image;
}

uint8_t ImageSOA_8bit::interpolate(PixelPair8 const & lowerPair, PixelPair8 const & upperPair,
                                   Weights const & weights) {
  uint8_t const color1 =
      lowerPair.lower +
      static_cast<uint8_t>(weights.xWeight * static_cast<float>(lowerPair.upper - lowerPair.lower));
  uint8_t const color2 =
      upperPair.lower +
      static_cast<uint8_t>(weights.xWeight * static_cast<float>(upperPair.upper - upperPair.lower));
  return static_cast<uint8_t>(static_cast<float>(color1) +
                              (weights.yWeight * static_cast<float>(color2 - color1)));
}

std::vector<uint8_t> ImageSOA_8bit::resizeChannel(Dimensions const & resizeData,
                                                  std::vector<uint8_t> const & originalChannel) {
  std::vector<uint8_t> resizedChannel(resizeData.width * resizeData.height);
  int const width_int  = static_cast<int>(gWidth());
  int const height_int = static_cast<int>(gHeight());

  for (size_t yPrime = 0; yPrime < resizeData.height; ++yPrime) {
    for (size_t xPrime = 0; xPrime < resizeData.width; ++xPrime) {
      float const normalizedX =
          (static_cast<float>(xPrime * gWidth()) / static_cast<float>(resizeData.width));
      float const normalizedY =
          (static_cast<float>(yPrime * gHeight()) / static_cast<float>(resizeData.height));

      int const lowerX = static_cast<int>(std::floor(normalizedX));
      int const upperX = std::min(static_cast<int>(std::ceil(normalizedX)), width_int - 1);
      int const lowerY = static_cast<int>(std::floor(normalizedY));
      int const upperY = std::min(static_cast<int>(std::ceil(normalizedY)), height_int - 1);

      // Get the four surrounding pixels
      PixelPair8 const lowerPair = {
        .lower = originalChannel[static_cast<uint8_t>((lowerY * width_int) + lowerX)],
        .upper = originalChannel[static_cast<uint8_t>((lowerY * width_int) + upperX)]};
      PixelPair8 const upperPair = {
        .lower = originalChannel[static_cast<uint8_t>((upperY * width_int) + lowerX)],
        .upper = originalChannel[static_cast<uint8_t>((upperY * width_int) + upperX)]};

      // Calculate interpolation weights
      Weights const weights = {.xWeight = normalizedX - static_cast<float>(lowerX),
                               .yWeight = normalizedY - static_cast<float>(lowerY)};

      // Interpolate and assign the value to the resized image
      resizedChannel[(yPrime * resizeData.width) + xPrime] =
          interpolate(lowerPair, upperPair, weights);
    }
  }
  return resizedChannel;
}

// Main function to resize the full image
void ImageSOA_8bit::resizeImage(Dimensions const & resizeData) {
  red   = resizeChannel(resizeData, red);
  green = resizeChannel(resizeData, green);
  blue  = resizeChannel(resizeData, blue);
}

uint16_t ImageSOA_16bit::interpolate(PixelPair16 const & lowerPair, PixelPair16 const & upperPair,
                                     Weights const & weights) {
  uint16_t const color1 =
      lowerPair.lower +
      static_cast<uint16_t>(weights.xWeight *
                            static_cast<float>(lowerPair.upper - lowerPair.lower));
  uint16_t const color2 =
      upperPair.lower +
      static_cast<uint16_t>(weights.xWeight *
                            static_cast<float>(upperPair.upper - upperPair.lower));
  return static_cast<uint16_t>(static_cast<float>(color1) +
                               (weights.yWeight * static_cast<float>(color2 - color1)));
}

std::vector<uint16_t> ImageSOA_16bit::resizeChannel(Dimensions const & resizeData,
                                                    std::vector<uint16_t> const & originalChannel) {
  std::vector<uint16_t> resizedChannel(resizeData.width * resizeData.height);
  int const width_int  = static_cast<int>(gWidth());
  int const height_int = static_cast<int>(gHeight());

  for (size_t yPrime = 0; yPrime < resizeData.height; ++yPrime) {
    for (size_t xPrime = 0; xPrime < resizeData.width; ++xPrime) {
      float const normalizedX =
          (static_cast<float>(xPrime * gWidth()) / static_cast<float>(resizeData.width));
      float const normalizedY =
          (static_cast<float>(yPrime * gHeight()) / static_cast<float>(resizeData.height));

      int const lowerX = static_cast<int>(std::floor(normalizedX));
      int const upperX = std::min(static_cast<int>(std::ceil(normalizedX)), width_int - 1);
      int const lowerY = static_cast<int>(std::floor(normalizedY));
      int const upperY = std::min(static_cast<int>(std::ceil(normalizedY)), height_int - 1);

      // Get the four surrounding pixels
      PixelPair16 const lowerPair = {
        .lower = originalChannel[static_cast<uint16_t>((lowerY * width_int) + lowerX)],
        .upper = originalChannel[static_cast<uint16_t>((lowerY * width_int) + upperX)]};
      PixelPair16 const upperPair = {
        .lower = originalChannel[static_cast<uint16_t>((upperY * width_int) + lowerX)],
        .upper = originalChannel[static_cast<uint16_t>((upperY * width_int) + upperX)]};

      // Calculate interpolation weights
      Weights const weights = {.xWeight = normalizedX - static_cast<float>(lowerX),
                               .yWeight = normalizedY - static_cast<float>(lowerY)};

      // Interpolate and assign the value to the resized image
      resizedChannel[(yPrime * resizeData.width) + xPrime] =
          interpolate(lowerPair, upperPair, weights);
    }
  }
  return resizedChannel;
}

// Main function to resize the full image
void ImageSOA_16bit::resizeImage(Dimensions const & resizeData) {
  red   = resizeChannel(resizeData, red);
  green = resizeChannel(resizeData, green);
  blue  = resizeChannel(resizeData, blue);
}