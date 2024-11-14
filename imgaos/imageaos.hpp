#ifndef IMAGEAOS_HPP
#define IMAGEAOS_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <tuple>

// Definición de la estructura Pixel
struct Pixel {
  uint16_t red;
  uint16_t green;
  uint16_t blue;

  bool operator==(const Pixel& other) const {
    return red == other.red && green == other.green && blue == other.blue;
  }
};

constexpr int ocho = 8;
constexpr int dieceiseis = 16;
// Hash function for Pixel
struct PixelHash {
  std::size_t operator()(const Pixel& pixel) const {
    return (static_cast<std::size_t>(pixel.red) << dieceiseis) | (static_cast<std::size_t>(pixel.green) << ocho) | static_cast<std::size_t>(pixel.blue);
  }
};

// Definición de la estructura PPMMetadata
struct PPMMetadata {
  std::string magicNumber;
  int width = 0;
  int height = 0;
  int maxColorValue = 0;
};

struct ImageMetadata {
  int width = 0;
  int height = 0;
  int maxColorValue = 0;
};

// Función que obtiene los metadatos de un archivo PPM
PPMMetadata getPPMMetadata(const std::string& filename);

// Función para escalar la intensidad de cada píxel al nuevo valor máximo
void scaleIntensity(std::vector<Pixel>& pixels, int currentMax, int newMax);

// Función para cargar una imagen PPM en un vector de píxeles
std::vector<Pixel> loadImage(const std::string& filename, const PPMMetadata& metadata);

// Función para guardar un vector de píxeles en un archivo PPM
void saveImage(const std::string& filename, const std::vector<Pixel>& pixels, const PPMMetadata& metadata);

// Función para redimensionar una imagen utilizando interpolación bilineal
std::vector<Pixel> resizeImage(const std::vector<Pixel>& originalPixels, const PPMMetadata& originalMetadata, int newWidth, int newHeight);

// Función para eliminar colores menos frecuentes de la imagen
std::vector<Pixel> removeLeastFrequentColors(const std::vector<Pixel>& pixels, int n);

// Función para calcular la distancia entre colores
int colorDistance(const Pixel& pixel1, const Pixel& pixel2);

// Función interpolate
Pixel interpolate(const Pixel& pixel1, const Pixel& pixel2, float weight);

#endif
