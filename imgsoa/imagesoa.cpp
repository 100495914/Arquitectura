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
  size_t const size = gWidth() * gHeight();

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
  size_t const size = gWidth() * gHeight();
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
  size_t const size = gWidth() * gHeight();
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
  size_t const size          = gWidth() * gHeight();
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
  size_t const size          = gWidth() * gHeight();

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

uint8_t ImageSOA_8bit::getInterpolatedPixel(double const x_var,
                                            std::vector<uint8_t> const & channel,
                                            double const y_var) const {
  auto x_one         = static_cast<size_t>(x_var);
  auto y_one         = static_cast<size_t>(y_var);
  size_t const x_two = std::min(x_one + 1, gWidth() - 1);
  size_t const y_two = std::min(y_one + 1, gHeight() - 1);

  double const f_x = x_var - static_cast<double>(x_one);
  double const f_y = y_var - static_cast<double>(y_one);

  uint8_t const p_1 = channel[(y_one * gWidth()) + x_one];
  uint8_t const p_2 = channel[(y_one * gWidth()) + x_two];
  uint8_t const p_3 = channel[(y_two * gWidth()) + x_one];
  uint8_t const p_4 = channel[(y_two * gWidth()) + x_two];

  return static_cast<uint8_t>(((1 - f_x) * (1 - f_y) * p_1) + (f_x * (1 - f_y) * p_2) +
                              ((1 - f_x) * f_y * p_3) + (f_x * f_y * p_4));
}

void ImageSOA_8bit::resizeChannel(std::vector<uint8_t> const & src, std::vector<uint8_t> & dst,
                                  Dimensions const dim) const {
  size_t const newWidth  = dim.width;
  size_t const newHeight = dim.height;
  double const scaleX    = static_cast<double>(gWidth()) / static_cast<double>(newWidth);
  double const scaleY    = static_cast<double>(gHeight()) / static_cast<double>(newHeight);

  for (size_t y_var = 0; y_var < newHeight; ++y_var) {
    for (size_t x_var = 0; x_var < newWidth; ++x_var) {
      double const srcX               = static_cast<double>(x_var) * scaleX;
      double const srcY               = static_cast<double>(y_var) * scaleY;
      dst[(y_var * newWidth) + x_var] = getInterpolatedPixel(srcX, src, srcY);
    }
  }
}

void ImageSOA_8bit::resize(Dimensions const dim) {
  // Create new vectors for resized image
  std::vector<uint8_t> newRed(dim.width * dim.height);
  std::vector<uint8_t> newGreen(dim.width * dim.height);
  std::vector<uint8_t> newBlue(dim.width * dim.height);

  // Resize each channel
  resizeChannel(red, newRed, dim);
  resizeChannel(green, newGreen, dim);
  resizeChannel(blue, newBlue, dim);

  // Replace old vectors with new ones
  red   = std::move(newRed);
  green = std::move(newGreen);
  blue  = std::move(newBlue);

  // Update dimensions
  sWidth(dim.width);
  sHeight(dim.height);
}

uint16_t ImageSOA_16bit::getInterpolatedPixel(double const x_var,
                                              std::vector<uint16_t> const & channel,
                                              double const y_var) const {
  auto const x_one   = static_cast<size_t>(x_var);
  auto const y_one   = static_cast<size_t>(y_var);
  size_t const x_two = std::min(x_one + 1, gWidth() - 1);
  size_t const y_two = std::min(y_one + 1, gHeight() - 1);

  double const f_x = x_var - static_cast<double>(x_one);
  double const f_y = y_var - static_cast<double>(y_one);

  uint16_t const p_1 = channel[(y_one * gWidth()) + x_one];
  uint16_t const p_2 = channel[(y_one * gWidth()) + x_two];
  uint16_t const p_3 = channel[(y_two * gWidth()) + x_one];
  uint16_t const p_4 = channel[(y_two * gWidth()) + x_two];

  return static_cast<uint16_t>(((1 - f_x) * (1 - f_y) * p_1) + (f_x * (1 - f_y) * p_2) +
                              ((1 - f_x) * f_y * p_3) + (f_x * f_y * p_4));
}

void ImageSOA_16bit::resizeChannel(std::vector<uint16_t> const & src, std::vector<uint16_t> & dst,
                                   Dimensions const dim) const {
  size_t const newWidth  = dim.width;
  size_t const newHeight = dim.height;
  double const scaleX    = static_cast<double>(gWidth()) / static_cast<double>(newWidth);
  double const scaleY    = static_cast<double>(gHeight()) / static_cast<double>(newHeight);

  for (size_t y_var = 0; y_var < newHeight; ++y_var) {
    for (size_t x_var = 0; x_var < newWidth; ++x_var) {
      double const srcX               = static_cast<double>(x_var) * scaleX;
      double const srcY               = static_cast<double>(y_var) * scaleY;
      dst[(y_var * newWidth) + x_var] = getInterpolatedPixel(srcX, src, srcY);
    }
  }
}

void ImageSOA_16bit::resize(Dimensions const dim) {
  // Create new vectors for resized image
  std::vector<uint16_t> newRed(dim.width * dim.height);
  std::vector<uint16_t> newGreen(dim.width * dim.height);
  std::vector<uint16_t> newBlue(dim.width * dim.height);

  // Resize each channel
  resizeChannel(red, newRed, dim);
  resizeChannel(green, newGreen, dim);
  resizeChannel(blue, newBlue, dim);

  // Replace old vectors with new ones
  red   = std::move(newRed);
  green = std::move(newGreen);
  blue  = std::move(newBlue);

  // Update dimensions
  sWidth(dim.width);
  sHeight(dim.height);
}