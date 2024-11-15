//
// Created by diego on 24/10/24.
//
#include "PPMMetadata.hpp"
#include <fstream>
#include <stdexcept>
#include <string>

PPMMetadata getPPMMetadata(const std::string& filename) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Error: No se pudo abrir el archivo.");
  }

  PPMMetadata metadata;

  // Leer el número mágico
  file >> metadata.magicNumber;
  if (metadata.magicNumber != "P6") {
    throw std::runtime_error("Error: Formato PPM inválido.");
  }

  // Leer la anchura, altura y valor máximo de color
  file >> metadata.width >> metadata.height >> metadata.maxColorValue;

  // Validar las dimensiones
  if (metadata.width <= 0 || metadata.height <= 0) {
    throw std::runtime_error("Error: Dimensiones inválidas.");
  }

  // Validar el valor máximo de color
  constexpr int MaxColorValue = 65535;
  if (metadata.maxColorValue <= 0 || metadata.maxColorValue > MaxColorValue) {
    throw std::runtime_error("Error: Valor máximo de color fuera de rango.");
  }

  // Ignorar el carácter de nueva línea después de los metadatos
  file.ignore(1);

  return metadata;
}
