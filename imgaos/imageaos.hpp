//
// Created by diego on 20/10/24.
//
// imagesoa.hpp
#ifndef IMAGEAOS_HPP
#define IMAGEAOS_HPP

#include <string>
#include <vector>

struct Pixel {
    int red;
    int green;
    int blue;
};

struct PPMMetadata {
  std::string magicNumber;
  int width;
  int height;
  int maxColorValue;
};

// Función que obtiene los metadatos de un archivo PPM
PPMMetadata getPPMMetadata(const std::string& filename);

// Función para escalar la intensidad de cada píxel al nuevo valor máximo
void scaleIntensity(std::vector<Pixel>& pixels, int currentMax, int newMax);

// Función para cargar una imagen PPM en un vector de píxeles
std::vector<Pixel> loadImage(const std::string& filename, int width, int height, int maxColorValue);

// Función para guardar un vector de píxeles en un archivo PPM
void saveImage(const std::string& filename, const std::vector<Pixel>& pixels, int width, int height, int maxColorValue);

#endif // IMAGEAOS_HPP
