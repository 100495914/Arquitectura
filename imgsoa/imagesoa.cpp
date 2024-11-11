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
#include <variant>

genericImageSOA loadImage(std::string const & filepath) {
  std::cerr << "Trying to open: " << filepath << '\n';
  std::ifstream file(filepath, std::ios::binary);
  if (!file.is_open()) { throw std::runtime_error("Failed to open file"); }

  PPMMetadata metadata;
  file >> metadata.magicNumber >> metadata.width >> metadata.height >> metadata.maxColorValue;
  file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  // Create the appropriate image object based on maxColorValue
  if (metadata.maxColorValue <= MAX_8BIT_VALUE) {
    // 8-bit image
    auto image = std::make_unique<ImageSOA_8bit>(metadata);
    image->loadData(file);
    return image;
  }
  if (metadata.maxColorValue <= MAX_16BIT_VALUE) {
    // 16-bit image
    auto image = std::make_unique<ImageSOA_16bit>(metadata);
    image->loadData(file);
    return image;
  }
  throw std::runtime_error("Unsupported max color value!");
}

void ImageSOA_8bit::loadData(std::ifstream & file) {
  if (!file.is_open()) { throw std::ios_base::failure("Failed to open the file."); }

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

std::unique_ptr<ImageSOA_16bit> ImageSOA_8bit::scaleIntensityChannelSizeChange(uint const newMax) {
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
    file.read(reinterpret_cast<char *>(&red),  // NOLINT(*-pro-type-reinterpret-cast)
              1);
    file.read(reinterpret_cast<char *>(&green),  // NOLINT(*-pro-type-reinterpret-cast)
              1);
    file.read(reinterpret_cast<char *>(&blue),  // NOLINT(*-pro-type-reinterpret-cast)
              1);

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

std::unique_ptr<ImageSOA_8bit> ImageSOA_16bit::scaleIntensityChannelSizeChange(uint const newMax) {
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

int main() {
  try {
    std::string const input  = "../input/test.ppm";
    std::string const output = "../output/test.ppm";

    // Load an image from file
    auto const generic_image = loadImage(input);

    // Use std::visit to operate on the actual image type
    std::visit(
        [output](auto && image) {
          // Perform image processing here (scaling, etc.)
          auto n_image = image->scaleIntensityChannelSizeChange(30000);

          // Save the newly created image (the scaled variant)
          if (n_image) {  // Only save if scaling produced a valid new image
            n_image->saveToFile(output);
          } else {
            throw std::runtime_error("Failed to scale intensity and create a new image.");
          }
        },
        generic_image);

  } catch (std::exception const & e) { std::cerr << "Error: " << e.what() << '\n'; }

  return 0;
}
