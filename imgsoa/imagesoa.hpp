//
// Created by diego on 20/10/24.
//

#ifndef IMAGESOA_HPP
#define IMAGESOA_HPP

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

// Para imagenes con pixeles de 8 bits
struct ImageSOA_8bit {
    std::vector<uint8_t> red, green, blue;

    explicit ImageSOA_8bit(size_t const size) : red(size), green(size), blue(size) { }
};

// Para imagenes con pixeles de 16 bits
struct ImageSOA_16bit {
    std::vector<uint16_t> red, green, blue;

    explicit ImageSOA_16bit(size_t const size) : red(size), green(size), blue(size) { }
};

constexpr int MAX_8BIT_VALUE  = 255;
constexpr int MAX_16BIT_VALUE = 65535;

struct PPMMetadata {
    std::string magicNumber;
    int width;
    int height;
    int maxColorValue;
};

struct imageDimensions {
    uint width;
    uint height;
};

// Función para escalar la intensidad de cada píxel al nuevo valor máximo
// NOTA: Se puede hacer con 8 y 16 bit con una funcion, pero queda mucho menos legible
void scaleIntensity(ImageSOA_8bit & pixels, int currentMax, int newMax);

void scaleIntensity(ImageSOA_16bit & pixels, int currentMax, int newMax);

std::variant<ImageSOA_8bit, ImageSOA_16bit> loadImage(std::string const & filename,
                                                      PPMMetadata & metadata);

PPMMetadata getPPMMetadata(std::string const & filename);

void resizeImage(ImageSOA_8bit & pixels, PPMMetadata & metadata, imageDimensions newDimensions);

void resizeImage(ImageSOA_16bit & pixels, PPMMetadata & metadata, imageDimensions newDimensions);

void saveImage(std::string const & filename, ImageSOA_8bit const & image,
               imageDimensions dimensions, int maxColorValue);

void saveImage(std::string const & filename, ImageSOA_16bit const & image,
               imageDimensions dimensions, int maxColorValue);
std::variant<ImageSOA_8bit, ImageSOA_16bit> removeLeastFrequentColors(ImageSOA_8bit const & imagen,
                                                                      int n);
std::variant<ImageSOA_8bit, ImageSOA_16bit> removeLeastFrequentColors(ImageSOA_16bit const & imagen,
                                                                      int n);
#endif  // IMAGESOA_HPP
