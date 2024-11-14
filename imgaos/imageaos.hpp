#ifndef IMAGEAOS_HPP
#define IMAGEAOS_HPP

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

// Definición de la estructura Pixel
struct Pixel {
    uint16_t red;
    uint16_t green;
    uint16_t blue;

    bool operator<(Pixel const & other) const {
      return std::tie(red, green, blue) < std::tie(other.red, other.green, other.blue);
    }
};

// Definición de la estructura PPMMetadata
struct PPMMetadata {
    std::string magicNumber;
    int width         = 0;
    int height        = 0;
    int maxColorValue = 0;
};

struct ImageMetadata {
    int width         = 0;
    int height        = 0;
    int maxColorValue = 0;
};

// Función que obtiene los metadatos de un archivo PPM
PPMMetadata getPPMMetadata(std::string const & filename);

// Función para escalar la intensidad de cada píxel al nuevo valor máximo
void scaleIntensity8bit(std::vector<Pixel> & pixels, int currentMax, int newMax);
void scaleIntensity(std::vector<Pixel> & pixels, int currentMax, int newMax);
// Función para cargar una imagen PPM en un vector de píxeles
std::vector<Pixel> loadImage(std::string const & filename, PPMMetadata const & metadata);

// Función para guardar un vector de píxeles en un archivo PPM
void saveImage(std::string const & filename, std::vector<Pixel> const & pixels,
               PPMMetadata const & metadata);

// Función para redimensionar una imagen utilizando interpolación bilineal
std::vector<Pixel> resizeImage(std::vector<Pixel> const & originalPixels,
                               PPMMetadata const & originalMetadata, int newWidth, int newHeight);

// Función para eliminar colores menos frecuentes de la imagen
std::vector<Pixel> removeLeastFrequentColors(std::vector<Pixel> const & pixels, int n);

// Función para calcular la distancia entre colores
float colorDistance(Pixel const & pixel1, Pixel const & pixel2);

// Función interpolate
Pixel interpolate(Pixel const & pixel1, Pixel const & pixel2, float weight);

#endif
