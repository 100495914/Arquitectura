//
// Created by diego on 20/10/24.
//
#include "imagesoa.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <stdexcept>
#include <unordered_map>

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

namespace {
  double interpolate(double const value1, double const value2, double const weight) {
    return value1 + (weight * (value2 - value1));
  }
}  // namespace

bool ImageSOA_8bit::operator==(ImageSOA_8bit const & other) const {
  // Check if the dimensions are the same
  if (this->gWidth() != other.gWidth() || this->gHeight() != other.gHeight() ||
      this->gMaxColorValue() != other.gMaxColorValue() ||
      this->gMagicNumber() != other.gMagicNumber()) {
    return false;
  }

  // Check if the color channels are the same size
  if (this->red.size() != other.red.size() || this->green.size() != other.green.size() ||
      this->blue.size() != other.blue.size()) {
    return false;
  }
  constexpr int max_pixel_offset = 3;
  // Check if the pixel data in each channel is similar within a difference of 5
  for (size_t i = 0; i < this->red.size(); ++i) {
    if (std::abs(this->red[i] - other.red[i]) >= max_pixel_offset ||
        std::abs(this->green[i] - other.green[i]) >= max_pixel_offset ||
        std::abs(this->blue[i] - other.blue[i]) >= max_pixel_offset) {
      std::cout << "Byte difference exceeds 3 at index: " << i << '\n';
      return false;
    }
  }

  // If all checks pass, the images are considered equal
  return true;
}

bool ImageSOA_16bit::operator==(ImageSOA_16bit const & other) const {
  // Check if the dimensions are the same
  if (this->gWidth() != other.gWidth() || this->gHeight() != other.gHeight() ||
      this->gMaxColorValue() != other.gMaxColorValue() ||
      this->gMagicNumber() != other.gMagicNumber()) {
    return false;
      }

  // Check if the color channels are the same size
  if (this->red.size() != other.red.size() || this->green.size() != other.green.size() ||
      this->blue.size() != other.blue.size()) {
    return false;
      }
  constexpr int max_pixel_offset = 3;
  // Check if the pixel data in each channel is similar within a difference of 5
  for (size_t i = 0; i < this->red.size(); ++i) {
    if (std::abs(this->red[i] - other.red[i]) >= max_pixel_offset ||
        std::abs(this->green[i] - other.green[i]) >= max_pixel_offset ||
        std::abs(this->blue[i] - other.blue[i]) >= max_pixel_offset) {
      std::cout << "Byte difference exceeds 5 at index: " << i << '\n';
      return false;
        }
  }

  // If all checks pass, the images are considered equal
  return true;
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

int ImageSOA_8bit::calculatePosition(Point const point, Dimensions dim) {
  int const var_x  = point.x_coord;
  int const var_y  = point.y_coord;
  int const width  = static_cast<int>(dim.width);
  int const height = static_cast<int>(dim.height);
  // Ensure the coordinates are within bounds
  if (var_x < 0 || var_x >= width || var_y < 0 || var_y >= height) {
    std::cerr << "Error: Coordinates out of bounds!" << '\n';
    return -1;  // Return an invalid index
  }

  // Calculate the position in the 1D array
  return ((var_y) *width) + (var_x);
}

void ImageSOA_8bit::resize(Dimensions const dim) {
  std::cout << dim.width << "   " << dim.height << '\n';
  // Replace the old channels with the new resized ones
  red   = resize_helper(red, dim);
  green = resize_helper(green, dim);
  blue  = resize_helper(blue, dim);
  sWidth(dim.width);
  sHeight(dim.height);
}

double ImageSOA_8bit::helper_resizeInterpolate(std::vector<uint8_t> & channel,
                                               Dimensions const original_dimensions,
                                               double const x_target, double const y_target) {
  auto const width  = static_cast<double>(original_dimensions.width);
  auto const height = static_cast<double>(original_dimensions.height);

  int const x_l = static_cast<int>(std::floor(x_target));
  int x_h       = static_cast<int>(std::ceil(x_target));
  int const y_l = static_cast<int>(std::floor(y_target));
  int y_h       = static_cast<int>(std::ceil(y_target));
  if (x_h >= static_cast<int>(width)) { x_h = x_l; }
  if (y_h >= static_cast<int>(height)) { y_h = y_l; }
  auto const index_xl_yl = static_cast<size_t>(
      calculatePosition(Point{.x_coord = x_l, .y_coord = y_l}, original_dimensions));
  auto const index_xh_yl = static_cast<size_t>(
      calculatePosition(Point{.x_coord = x_h, .y_coord = y_l}, original_dimensions));
  auto const index_xl_yh = static_cast<size_t>(
      calculatePosition(Point{.x_coord = x_l, .y_coord = y_h}, original_dimensions));
  auto const index_xh_yh = static_cast<size_t>(
      calculatePosition(Point{.x_coord = x_h, .y_coord = y_h}, original_dimensions));
  int const value_xl_yl = channel[index_xl_yl];
  int const value_xh_yl = channel[index_xh_yl];
  int const value_xl_yh = channel[index_xl_yh];
  int const value_xh_yh = channel[index_xh_yh];
  double weight_x       = 0;
  if (x_h - x_l != 0) { weight_x = (x_target - x_l) / (x_h - x_l); }
  double const color1 = interpolate(value_xl_yl, value_xh_yl, weight_x);
  double const color2 = interpolate(value_xl_yh, value_xh_yh, weight_x);
  double weight_y     = 0;
  if (y_h - y_l != 0) { weight_y = (y_target - y_l) / (y_h - y_l); }
  double const interpolated_pixel = interpolate(color1, color2, weight_y);
  return interpolated_pixel;
}

std::vector<uint8_t> ImageSOA_8bit::resize_helper(std::vector<uint8_t> & channel,
                                                  Dimensions const dim) const {
  auto const new_width           = static_cast<double>(dim.width);
  auto const new_height          = static_cast<double>(dim.height);
  auto const original_dimensions = Dimensions{.width = gWidth(), .height = gHeight()};
  auto const width               = static_cast<double>(original_dimensions.width);
  auto const height              = static_cast<double>(original_dimensions.height);
  auto const new_size            = static_cast<size_t>(new_width * new_height);
  std::vector<uint8_t> new_channel(new_size);
  double const width_div  = ((width - 1) / (new_width-1));
  double const height_div = ((height - 1) / (new_height-1));
  for (size_t new_y = 0; new_y < static_cast<size_t>(new_height); new_y++) {
    for (size_t new_x = 0; new_x < static_cast<size_t>(new_width); new_x++) {
      double const x_target = static_cast<double>(new_x) * width_div;

      double const y_target     = (static_cast<double>(new_y) * height_div);
      double interpolated_pixel = 0;
      if (std::floor(x_target) != x_target || std::floor(y_target) != y_target) {
        interpolated_pixel =
            helper_resizeInterpolate(channel, original_dimensions, x_target, y_target);
      } else {
        interpolated_pixel = channel[static_cast<size_t>(calculatePosition(
            Point{.x_coord = static_cast<int>(x_target), .y_coord = static_cast<int>(y_target)},
            original_dimensions))];
      }
      auto const new_point =
          Point{.x_coord = static_cast<int>(new_x), .y_coord = static_cast<int>(new_y)};
      auto const final_index   = static_cast<size_t>(calculatePosition(new_point, dim));
      new_channel[final_index] = static_cast<uint8_t>(std::round(interpolated_pixel));
    }
  }
  return new_channel;
}

int ImageSOA_16bit::calculatePosition(Point const point, Dimensions dim) {
  int const var_x  = point.x_coord;
  int const var_y  = point.y_coord;
  int const width  = static_cast<int>(dim.width);
  int const height = static_cast<int>(dim.height);
  // Ensure the coordinates are within bounds
  if (var_x < 0 || var_x >= width || var_y < 0 || var_y >= height) {
    std::cerr << "Error: Coordinates out of bounds!" << '\n';
    return -1;  // Return an invalid index
  }

  // Calculate the position in the 1D array
  return ((var_y) *width) + (var_x);
}

void ImageSOA_16bit::resize(Dimensions const dim) {
  std::cout << dim.width << "   " << dim.height << '\n';
  // Replace the old channels with the new resized ones
  red   = resize_helper(red, dim);
  green = resize_helper(green, dim);
  blue  = resize_helper(blue, dim);
  sWidth(dim.width);
  sHeight(dim.height);
}

double ImageSOA_16bit::helper_resizeInterpolate(std::vector<uint16_t> & channel,
                                                Dimensions const original_dimensions,
                                                double const x_target, double const y_target) {
  auto const width  = static_cast<double>(original_dimensions.width);
  auto const height = static_cast<double>(original_dimensions.height);

  int const x_l = static_cast<int>(std::floor(x_target));
  int x_h       = static_cast<int>(std::ceil(x_target));
  int const y_l = static_cast<int>(std::floor(y_target));
  int y_h       = static_cast<int>(std::ceil(y_target));
  if (x_h >= static_cast<int>(width)) { x_h = x_l; }
  if (y_h >= static_cast<int>(height)) { y_h = y_l; }
  auto const index_xl_yl = static_cast<size_t>(
      calculatePosition(Point{.x_coord = x_l, .y_coord = y_l}, original_dimensions));
  auto const index_xh_yl = static_cast<size_t>(
      calculatePosition(Point{.x_coord = x_h, .y_coord = y_l}, original_dimensions));
  auto const index_xl_yh = static_cast<size_t>(
      calculatePosition(Point{.x_coord = x_l, .y_coord = y_h}, original_dimensions));
  auto const index_xh_yh = static_cast<size_t>(
      calculatePosition(Point{.x_coord = x_h, .y_coord = y_h}, original_dimensions));
  int const value_xl_yl = channel[index_xl_yl];
  int const value_xh_yl = channel[index_xh_yl];
  int const value_xl_yh = channel[index_xl_yh];
  int const value_xh_yh = channel[index_xh_yh];
  double weight_x       = 0;
  if (x_h - x_l != 0) { weight_x = (x_target - x_l) / (x_h - x_l); }
  double const color1 = interpolate(value_xl_yl, value_xh_yl, weight_x);
  double const color2 = interpolate(value_xl_yh, value_xh_yh, weight_x);
  double weight_y     = 0;
  if (y_h - y_l != 0) { weight_y = (y_target - y_l) / (y_h - y_l); }
  double const interpolated_pixel = interpolate(color1, color2, weight_y);
  return interpolated_pixel;
}

std::vector<uint16_t> ImageSOA_16bit::resize_helper(std::vector<uint16_t> & channel,
                                                    Dimensions const dim) const {
  auto const new_width           = static_cast<double>(dim.width);
  auto const new_height          = static_cast<double>(dim.height);
  auto const original_dimensions = Dimensions{.width = gWidth(), .height = gHeight()};
  auto const width               = static_cast<double>(original_dimensions.width);
  auto const height              = static_cast<double>(original_dimensions.height);
  auto const new_size            = static_cast<size_t>(new_width * new_height);
  std::vector<uint16_t> new_channel(new_size);
  double const width_div  = (width / new_width);
  double const height_div = (height / new_height);
  for (size_t new_y = 0; new_y < static_cast<size_t>(new_height); new_y++) {
    for (size_t new_x = 0; new_x < static_cast<size_t>(new_width); new_x++) {
      double const x_target = static_cast<double>(new_x) * width_div;

      double const y_target     = (static_cast<double>(new_y) * height_div);
      double interpolated_pixel = 0;
      if (std::floor(x_target) != x_target || std::floor(y_target) != y_target) {
        interpolated_pixel =
            helper_resizeInterpolate(channel, original_dimensions, x_target, y_target);
      } else {
        interpolated_pixel = channel[static_cast<size_t>(calculatePosition(
            Point{.x_coord = static_cast<int>(x_target), .y_coord = static_cast<int>(y_target)},
            original_dimensions))];
      }
      auto const new_point =
          Point{.x_coord = static_cast<int>(new_x), .y_coord = static_cast<int>(new_y)};
      auto const final_index   = static_cast<size_t>(calculatePosition(new_point, dim));
      new_channel[final_index] = static_cast<uint8_t>(std::round(interpolated_pixel));
    }
  }
  return new_channel;
}

void ImageSOA_8bit::reduceColors(size_t n) {
  auto frequencies = computeColorFrequencies();

  // If the number of colors to reduce is greater than or equal to the total number of colors,
  // return early
  if (n >= frequencies.size()) { return; }

  // Get the least frequent colors
  auto colorsToRemove = findLeastFrequentColors(frequencies, n);

  // Create a set for valid colors
  std::unordered_set<RGB8> validColors;
  for (auto const & [color, freq] : frequencies) { validColors.insert(color); }

  // Remove the least frequent colors from the valid color set
  for (auto const & color : colorsToRemove) { validColors.erase(color); }

  // Precompute nearest colors for the least frequent colors
  std::unordered_map<RGB8, RGB8> replacementMap;
  replacementMap.reserve(colorsToRemove.size());  // Reserve memory for efficiency

  // Compute nearest colors and fill the replacement map
  for (auto const & color : colorsToRemove) {
    // Finding the closest color from the valid colors
    RGB8 const closestColor = findNearestColor(color, validColors);
    replacementMap[color]   = closestColor;
  }

  // Replace the colors in the image with the new mapped colors
  replaceColors(replacementMap);
}

std::unordered_map<RGB8, size_t> ImageSOA_8bit::computeColorFrequencies() const {
  std::unordered_map<RGB8, size_t> frequencies;
  size_t const size = gWidth() * gHeight();

  for (size_t i = 0; i < size; ++i) {
    RGB8 const color{.r = red[i], .g = green[i], .b = blue[i]};
    frequencies[color]++;
  }
  return frequencies;
}

std::vector<RGB8>
    ImageSOA_8bit::findLeastFrequentColors(std::unordered_map<RGB8, size_t> const & freqs,
                                           size_t const n) {
  auto comp = [](std::pair<RGB8, size_t> const & var_a, std::pair<RGB8, size_t> const & var_b) {
    if (var_a.second != var_b.second) { return var_a.second < var_b.second; }  // Change > to <
    if (var_a.first.b != var_b.first.b) { return var_a.first.b < var_b.first.b; }
    if (var_a.first.g != var_b.first.g) { return var_a.first.g < var_b.first.g; }
    return var_a.first.r < var_b.first.r;
  };

  std::priority_queue<std::pair<RGB8, size_t>, std::vector<std::pair<RGB8, size_t>>, decltype(comp)>
      priority_queue(comp);

  for (auto const & [color, freq] : freqs) {
    priority_queue.emplace(color, freq);
    if (priority_queue.size() > n) { priority_queue.pop(); }
  }

  std::vector<RGB8> result;
  while (!priority_queue.empty()) {
    result.push_back(priority_queue.top().first);
    priority_queue.pop();
  }
  return result;
}

double ImageSOA_8bit::colorDistance(const RGB8 & var_c1, const RGB8 & var_c2) {
  double const var_dr = var_c1.r - var_c2.r;
  double const var_dg = var_c1.g - var_c2.g;
  double const var_db = var_c1.b - var_c2.b;
  return (var_dr * var_dr) + (var_dg * var_dg) + (var_db * var_db);
}

RGB8 ImageSOA_8bit::findNearestColor(const RGB8 & target,
                                     std::unordered_set<RGB8> const & validColors) {
  // Initialize nearest color and minimum distance with the first color
  RGB8 nearest   = *validColors.begin();
  double minDist = colorDistance(target, nearest);

  // Loop through each valid color and find the nearest color
  for (auto const & color : validColors) {
    double const dist = colorDistance(target, color);
    if (dist < minDist) {
      minDist = dist;
      nearest = color;
    }
  }
  return nearest;
}

void ImageSOA_8bit::replaceColors(std::unordered_map<RGB8, RGB8> const & colorMap) {
  size_t const size = gWidth() * gHeight();
  for (size_t i = 0; i < size; ++i) {
    RGB8 const color{.r = red[i], .g = green[i], .b = blue[i]};
    if (auto found_color = colorMap.find(color); found_color != colorMap.end()) {
      red[i]   = found_color->second.r;
      green[i] = found_color->second.g;
      blue[i]  = found_color->second.b;
    }
  }
}

// 16-bit implementations follow the same pattern
void ImageSOA_16bit::reduceColors(size_t n) {
  auto frequencies = computeColorFrequencies();
  if (n >= frequencies.size()) { return; }

  auto colorsToRemove = findLeastFrequentColors(frequencies, n);
  std::unordered_set<RGB16> validColors;
  for (auto const & [color, freq] : frequencies) { validColors.insert(color); }
  for (auto const & color : colorsToRemove) { validColors.erase(color); }

  std::unordered_map<RGB16, RGB16> replacementMap;
  for (auto const & color : colorsToRemove) {
    replacementMap[color] = findNearestColor(color, validColors);
  }

  replaceColors(replacementMap);
}

std::unordered_map<RGB16, size_t> ImageSOA_16bit::computeColorFrequencies() const {
  std::unordered_map<RGB16, size_t> frequencies;
  size_t const size = gWidth() * gHeight();

  for (size_t i = 0; i < size; ++i) {
    RGB16 const color{.r = red[i], .g = green[i], .b = blue[i]};
    frequencies[color]++;
  }
  return frequencies;
}

std::vector<RGB16>
    ImageSOA_16bit::findLeastFrequentColors(std::unordered_map<RGB16, size_t> const & freqs,
                                            size_t const n) {
  auto comp = [](std::pair<RGB16, size_t> const & var_a, std::pair<RGB16, size_t> const & var_b) {
    if (var_a.second != var_b.second) { return var_a.second < var_b.second; }  // Change > to <
    if (var_a.first.b != var_b.first.b) { return var_a.first.b < var_b.first.b; }
    if (var_a.first.g != var_b.first.g) { return var_a.first.g < var_b.first.g; }
    return var_a.first.r < var_b.first.r;
  };

  std::priority_queue<std::pair<RGB16, size_t>, std::vector<std::pair<RGB16, size_t>>,
                      decltype(comp)>
      priority_queue(comp);

  for (auto const & [color, freq] : freqs) {
    priority_queue.emplace(color, freq);
    if (priority_queue.size() > n) { priority_queue.pop(); }
  }

  std::vector<RGB16> result;
  while (!priority_queue.empty()) {
    result.push_back(priority_queue.top().first);
    priority_queue.pop();
  }
  return result;
}

double ImageSOA_16bit::colorDistance(const RGB16 & var_c1, const RGB16 & var_c2) {
  double const var_dr = var_c1.r - var_c2.r;
  double const var_dg = var_c1.g - var_c2.g;
  double const var_db = var_c1.b - var_c2.b;
  return (var_dr * var_dr) + (var_dg * var_dg) + (var_db * var_db);
}

RGB16 ImageSOA_16bit::findNearestColor(const RGB16 & target,
                                       std::unordered_set<RGB16> const & validColors) {
  RGB16 nearest  = *validColors.begin();
  double minDist = colorDistance(target, nearest);

  for (auto const & color : validColors) {
    if (double const dist = colorDistance(target, color); dist < minDist) {
      minDist = dist;
      nearest = color;
    }
  }
  return nearest;
}

void ImageSOA_16bit::replaceColors(std::unordered_map<RGB16, RGB16> const & colorMap) {
  size_t const size = gWidth() * gHeight();
  for (size_t i = 0; i < size; ++i) {
    RGB16 const color{.r = red[i], .g = green[i], .b = blue[i]};
    auto found_color = colorMap.find(color);
    if (found_color != colorMap.end()) {
      red[i]   = found_color->second.r;
      green[i] = found_color->second.g;
      blue[i]  = found_color->second.b;
    }
  }
}