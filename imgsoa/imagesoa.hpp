//
// Created by diego on 20/10/24.
//

#ifndef IMAGESOA_HPP
#define IMAGESOA_HPP

#include <string>
#include <vector>
#include <cstdint>

// Para imagenes con pixeles de 8 bits
struct ImageSOA_8bit{
  std::vector<uint8_t> red;    // Vector de valores rojos
  std::vector<uint8_t> green;  // Vector de valores verdes
  std::vector<uint8_t> blue;   // Vector de valores azules

  // Inicializar los vectores a un tamaño determinado
  ImageSOA_8bit(size_t vector_size) : red(vector_size), green(vector_size), blue(vector_size) {}
};

// Para imagenes con pixeles de 16 bits
struct ImageSOA_16bit {
  std::vector<uint16_t> red;    // Vector de valores rojos
  std::vector<uint16_t> green;  // Vector de valores verdes
  std::vector<uint16_t> blue;   // Vector de valores azules

  // Inicializar los vectores a un tamaño determinado
  ImageSOA_16bit(size_t vector_size) : red(vector_size), green(vector_size), blue(vector_size) {}
};

struct PPMMetadata {
  std::string magicNumber;
  int width;
  int height;
  int maxColorValue;
};

// Función para escalar la intensidad de cada píxel al nuevo valor máximo
// NOTA: Se puede hacer con 8 y 16 bit con una funcion, pero queda mucho menos legible
void scaleIntensity(ImageSOA_8bit& pixels, int currentMax, int newMax);

void scaleIntensity(ImageSOA_16bit& pixels, int currentMax, int newMax);

ImageSOA_8bit loadImage(const std::string& filename, int width, int height, int maxColorValue);


#endif IMAGESOA_HPP

