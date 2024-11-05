//
// Created by diego on 2/11/24.
//
#include <iostream>
#include "../imgaos/imageaos.hpp" // Asegúrate de que imageaos.hpp esté configurado con loadImage y saveImage

int main() {
  try {
    // Parámetros de la imagen de prueba
    std::string inputFile = "../input/deer-large.ppm";  // Archivo PPM de entrada
    std::string outputFile = "../output/scaled_image.ppm";  // Archivo PPM de salida
    int newMaxLevel = 128;  // Nuevo nivel máximo de intensidad

    // Cargar la imagen PPM
    PPMMetadata metadata = getPPMMetadata(inputFile);
    std::vector<Pixel> pixels = loadImage(inputFile, metadata.width, metadata.height, metadata.maxColorValue);

    // Aplicar el escalado de intensidad
    scaleIntensity(pixels, metadata.maxColorValue, newMaxLevel);

    // Guardar la imagen con el nuevo nivel de intensidad
    saveImage(outputFile, pixels, metadata.width, metadata.height, newMaxLevel);

    std::cout << "Imagen escalada y guardada en: " << outputFile << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}
