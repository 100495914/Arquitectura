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

  // Validar el valor máximo de color
  if (metadata.maxColorValue <= 0 || metadata.maxColorValue > 65535) {
    throw std::runtime_error("Error: Valor máximo de color fuera de rango.");
  }

  // Ignorar el carácter de nueva línea después de los metadatos
  file.ignore(1);

  return metadata;
}
