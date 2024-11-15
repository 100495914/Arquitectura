#include "imageaos.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

void scaleIntensity(std::vector<Pixel>& pixels, int currentMax, int newMax) {
  for (auto& pixel : pixels) {
    pixel.red = static_cast<uint16_t>(std::trunc(pixel.red * newMax) / static_cast<double>(currentMax));
    pixel.green = static_cast<uint16_t>(std::trunc(pixel.green * newMax )/ static_cast<double>(currentMax));
    pixel.blue = static_cast<uint16_t>(std::trunc(pixel.blue * newMax) / static_cast<double>(currentMax));
  }
}

// Función para interpolar un solo canal de color entre dos valores
double interpolateChannel(double channel1, double channel2, double weight) {
    return channel1 + (weight * (channel2 - channel1));
}

// Interpolación de un píxel usando double para mayor precisión
Pixel interpolate(const Pixel& pixel1, const Pixel& pixel2, double weight) {
    Pixel result = {.red = 0, .green = 0, .blue = 0};
    result.red = static_cast<uint16_t>(std::round(interpolateChannel(static_cast<double>(pixel1.red), static_cast<double>(pixel2.red), weight)));
    result.green = static_cast<uint16_t>(std::round(interpolateChannel(static_cast<double>(pixel1.green), static_cast<double>(pixel2.green), weight)));
    result.blue = static_cast<uint16_t>(std::round(interpolateChannel(static_cast<double>(pixel1.blue), static_cast<double>(pixel2.blue), weight)));
    return result;
}

// Implementación de la función para redimensionar la imagen con ajustes
// Implementación de la función para redimensionar la imagen con ajustes
std::vector<Pixel> resizeImage(const std::vector<Pixel>& originalPixels, const PPMMetadata& originalMetadata, int newWidth, int newHeight) {
    std::vector<Pixel> resizedPixels(static_cast<size_t>(newWidth) * static_cast<size_t>(newHeight));

    const double xRatio = static_cast<double>(originalMetadata.width - 1) / static_cast<double>(newWidth - 1);
    const double yRatio = static_cast<double>(originalMetadata.height - 1) / static_cast<double>(newHeight -1);

    for (int yPrime = 0; yPrime < newHeight; ++yPrime) {
        for (int xPrime = 0; xPrime < newWidth; ++xPrime) {
            const double normalizedX = xPrime * xRatio;
            const double normalizedY = yPrime * yRatio;

            auto lowerX = static_cast<size_t>(std::floor(normalizedX));
            const size_t upperX = std::min(lowerX + 1, static_cast<size_t>(originalMetadata.width - 1));
            auto lowerY = static_cast<size_t>(std::floor(normalizedY));
            const size_t upperY = std::min(lowerY + 1, static_cast<size_t>(originalMetadata.height - 1));

            const Pixel& pixel1 = originalPixels[(lowerY * static_cast<size_t>(originalMetadata.width)) + lowerX];
            const Pixel& pixel2 = originalPixels[(lowerY * static_cast<size_t>(originalMetadata.width)) + upperX];
            const Pixel& pixel3 = originalPixels[(upperY * static_cast<size_t>(originalMetadata.width)) + lowerX];
            const Pixel& pixel4 = originalPixels[(upperY * static_cast<size_t>(originalMetadata.width)) + upperX];

            const double xWeight = normalizedX - static_cast<double>(lowerX);
            const double yWeight = normalizedY - static_cast<double>(lowerY);

            const Pixel color1 = interpolate(pixel1, pixel2, xWeight);
            const Pixel color2 = interpolate(pixel3, pixel4, xWeight);
            const Pixel finalColor = interpolate(color1, color2, yWeight);

            resizedPixels[(static_cast<size_t>(yPrime) * static_cast<size_t>(newWidth)) + static_cast<size_t>(xPrime)] = finalColor;
        }
    }
    return resizedPixels;
}



// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
// Implementación de la función para guardar la imagen
// Implementación de la función para guardar la imagen
void saveImage(const std::string& filename, const std::vector<Pixel>& pixels, const PPMMetadata& metadata) {
  std::ofstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: " + filename);
  }

  file << "P6\n" << metadata.width << " " << metadata.height << "\n" << metadata.maxColorValue << "\n";
  constexpr int maxValue = 256;
  const int bytesPerChannel = (metadata.maxColorValue < maxValue) ? 1 : 2;

  for (const auto& pixel : pixels) {
    if (bytesPerChannel == 1) {
      auto red = static_cast<unsigned char>(pixel.red);
      auto green = static_cast<unsigned char>(pixel.green);
      auto blue = static_cast<unsigned char>(pixel.blue);

      file.write(reinterpret_cast<char*>(&red), 1);
      file.write(reinterpret_cast<char*>(&green), 1);
      file.write(reinterpret_cast<char*>(&blue), 1);
    } else {
      auto red = static_cast<unsigned short>(pixel.red);
      auto green = static_cast<unsigned short>(pixel.green);
      auto blue = static_cast<unsigned short>(pixel.blue);

      file.write(reinterpret_cast<char*>(&red), 2);
      file.write(reinterpret_cast<char*>(&green), 2);
      file.write(reinterpret_cast<char*>(&blue), 2);
    }
  }

  file.close();
}




// Implementación de la función para cargar la imagen
// Implementación de la función para cargar la imagen
std::vector<Pixel> loadImage(const std::string& filename, const PPMMetadata& metadata) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {throw std::runtime_error("Unable to open file: " + filename);}

  std::string magicNumber;
  file >> magicNumber;
  file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignora la línea completa después del magicNumber
  PPMMetadata mdata;
  file >> mdata.width >> mdata.height >> mdata.maxColorValue;
  file.ignore(1, '\n');
  std::vector<Pixel> pixels(static_cast<std::vector<Pixel>::size_type>(metadata.width) * static_cast<std::vector<Pixel>::size_type>(metadata.height));

  constexpr int maxValue = 256;
  const int bytesPerChannel = (metadata.maxColorValue < maxValue) ? 1 : 2;

  for (auto& pixel : pixels) {
    if (bytesPerChannel == 1) {
      unsigned char red = 0;
      unsigned char green = 0;
      unsigned char blue = 0;
      file.read(reinterpret_cast<char*>(&red), 1);
      file.read(reinterpret_cast<char*>(&green), 1);
      file.read(reinterpret_cast<char*>(&blue), 1);
      pixel.red = red;
      pixel.green = green;
      pixel.blue = blue;
    } else {
      unsigned short red = 0;
      unsigned short green = 0;
      unsigned short blue = 0;
      file.read(reinterpret_cast<char*>(&red), 2);
      file.read(reinterpret_cast<char*>(&green), 2);
      file.read(reinterpret_cast<char*>(&blue), 2);
      pixel.red = static_cast<uint8_t>(red);
      pixel.green = static_cast<uint8_t>(green);
      pixel.blue = static_cast<uint8_t>(blue);
    }
  }
  file.close();
  return pixels;
}


// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)


// Función para obtener metadatos de PPM
PPMMetadata getPPMMetadata(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    PPMMetadata metadata;
    file >> metadata.magicNumber;
    file >> metadata.width >> metadata.height >> metadata.maxColorValue;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    file.close();

    return metadata;
}

// Función para calcular la distancia entre colores
// Función para calcular la distancia entre colores
// Function to calculate the distance between colors
int colorDistance(const Pixel& pixel1, const Pixel& pixel2) {
  return ((pixel1.red - pixel2.red) * (pixel1.red - pixel2.red)) +
         ((pixel1.green - pixel2.green) * (pixel1.green - pixel2.green)) +
         ((pixel1.blue - pixel2.blue) * (pixel1.blue - pixel2.blue));
}

std::vector<Pixel> removeLeastFrequentColors(const std::vector<Pixel>& pixels, int n) {
  if (n < 0) {throw std::invalid_argument("El número de colores a eliminar no puede ser negativo.");}

  // Contar la frecuencia de colores
  std::unordered_map<Pixel, int, PixelHash> colorFrequency;
  for (const auto& pixel : pixels) {
    colorFrequency[pixel]++;
  }

  // Convertir a vector y ordenar por frecuencia
  std::vector<std::pair<Pixel, int>> colorFreqVec(colorFrequency.begin(), colorFrequency.end());
  std::ranges::sort(colorFreqVec, [](const auto& aaa, const auto& bbb) {
    return aaa.second < bbb.second;
  });

  // Mantener los colores más frecuentes
  std::unordered_map<Pixel, Pixel, PixelHash> colorReplacement;
  for (size_t i = 0; i < static_cast<size_t>(n) && i < colorFreqVec.size(); ++i) {
    const Pixel& colorToRemove = colorFreqVec[i].first;
    int minDistance = std::numeric_limits<int>::max();
    Pixel closestColor = colorFreqVec[i].first;

    // Encontrar el color más cercano
    for (auto it = colorFreqVec.begin() + n; it != colorFreqVec.end(); ++it) {
      const int distance = colorDistance(colorToRemove, it->first);
      if (distance < minDistance) {
        minDistance = distance;
        closestColor = it->first;
      }
    }
    colorReplacement[colorToRemove] = closestColor;
  }
  // Aplicar reemplazos
  std::vector<Pixel> modifiedPixels = pixels;
  for (auto& pixel : modifiedPixels) {
    if (colorReplacement.contains(pixel)) {
      pixel = colorReplacement[pixel];
    }
  }
  return modifiedPixels;
}


