#ifndef IMAGEAOS_HPP
#define IMAGEAOS_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <tuple>

// Definición de la estructura Pixel
struct Pixel {
  uint8_t red;
  uint8_t green;
  uint8_t blue;

  bool operator<(const Pixel& other) const {
    return std::tie(red, green, blue) < std::tie(other.red, other.green, other.blue);
  }
};

// Definición de la estructura PPMMetadata
struct PPMMetadata {
  std::string magicNumber;
  int width = 0;
  int height = 0;
  int maxColorValue = 0;
};

// Función que obtiene los metadatos de un archivo PPM
PPMMetadata getPPMMetadata(const std::string& filename);

// Función para escalar la intensidad de cada píxel al nuevo valor máximo
void scaleIntensity(std::vector<Pixel>& pixels, int currentMax, int newMax);

// Función para cargar una imagen PPM en un vector de píxeles
std::vector<Pixel> loadImage(const std::string& filename, int width, int height, int maxColorValue);

// Función para guardar un vector de píxeles en un archivo PPM
void saveImage(const std::string& filename, const std::vector<Pixel>& pixels, int width, int height, int maxColorValue);

// Función para redimensionar una imagen utilizando interpolación bilineal
std::vector<Pixel> resizeImage(const std::vector<Pixel>& originalPixels, int originalWidth, int originalHeight, int newWidth, int newHeight);

// Función para eliminar colores menos frecuentes de la imagen
std::vector<Pixel> removeLeastFrequentColors(const std::vector<Pixel>& pixels, int n);

#endif // IMAGEAOS_HPP
