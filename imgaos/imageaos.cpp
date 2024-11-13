#include "imageaos.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cmath>
#include <map>
#include <algorithm>
#include <set>

void scaleIntensity(std::vector<Pixel>& pixels, int currentMax, int newMax) {
    for (auto& pixel : pixels) {
        pixel.red = static_cast<uint8_t>(std::round(pixel.red * newMax / static_cast<double>(currentMax)));
        pixel.green = static_cast<uint8_t>(std::round(pixel.green * newMax / static_cast<double>(currentMax)));
        pixel.blue = static_cast<uint8_t>(std::round(pixel.blue * newMax / static_cast<double>(currentMax)));
    }
}

// Implementación de la función interpolate
Pixel interpolate(const Pixel& pixel1, const Pixel& pixel2, float weight) {
    Pixel result = {.red = 0, .green = 0, .blue = 0};
    result.red = static_cast<uint8_t>(std::round(static_cast<float>(pixel1.red) + (weight * (static_cast<float>(pixel2.red) - static_cast<float>(pixel1.red)))));
    result.green = static_cast<uint8_t>(std::round(static_cast<float>(pixel1.green) + (weight * (static_cast<float>(pixel2.green) - static_cast<float>(pixel1.green)))));
    result.blue = static_cast<uint8_t>(std::round(static_cast<float>(pixel1.blue) + (weight * (static_cast<float>(pixel2.blue) - static_cast<float>(pixel1.blue)))));
    return result;
}

// Implementación de la función para redimensionar la imagen
std::vector<Pixel> resizeImage(const std::vector<Pixel>& originalPixels, const PPMMetadata& originalMetadata, int newWidth, int newHeight) {
    std::vector<Pixel> resizedPixels(static_cast<std::vector<Pixel>::size_type>(newWidth) * static_cast<std::vector<Pixel>::size_type>(newHeight));
    for (int yPrime = 0; yPrime < newHeight; ++yPrime) {
        for (int xPrime = 0; xPrime < newWidth; ++xPrime) {
            const float normalizedX = (static_cast<float>(xPrime) * static_cast<float>(originalMetadata.width)) / static_cast<float>(newWidth);
            const float normalizedY = (static_cast<float>(yPrime) * static_cast<float>(originalMetadata.height)) / static_cast<float>(newHeight);

            const int lowerX = static_cast<int>(std::floor(normalizedX));
            int upperX = static_cast<int>(std::ceil(normalizedX));
            const int lowerY = static_cast<int>(std::floor(normalizedY));
            int upperY = static_cast<int>(std::ceil(normalizedY));

            if (upperX >= originalMetadata.width) {
                upperX = originalMetadata.width - 1;
            }
            if (upperY >= originalMetadata.height) {
                upperY = originalMetadata.height - 1;
            }

            const Pixel pixel1 = originalPixels[(static_cast<std::vector<Pixel>::size_type>(lowerY) * static_cast<std::vector<Pixel>::size_type>(originalMetadata.width)) + static_cast<std::vector<Pixel>::size_type>(lowerX)];
            const Pixel pixel2 = originalPixels[(static_cast<std::vector<Pixel>::size_type>(lowerY) * static_cast<std::vector<Pixel>::size_type>(originalMetadata.width)) + static_cast<std::vector<Pixel>::size_type>(upperX)];
            const Pixel pixel3 = originalPixels[(static_cast<std::vector<Pixel>::size_type>(upperY) * static_cast<std::vector<Pixel>::size_type>(originalMetadata.width)) + static_cast<std::vector<Pixel>::size_type>(lowerX)];
            const Pixel pixel4 = originalPixels[(static_cast<std::vector<Pixel>::size_type>(upperY) * static_cast<std::vector<Pixel>::size_type>(originalMetadata.width)) + static_cast<std::vector<Pixel>::size_type>(upperX)];

            const float xWeight = normalizedX - static_cast<float>(lowerX);
            const float yWeight = normalizedY - static_cast<float>(lowerY);

            const Pixel color1 = interpolate(pixel1, pixel2, xWeight);
            const Pixel color2 = interpolate(pixel3, pixel4, xWeight);
            const Pixel finalColor = interpolate(color1, color2, yWeight);

            resizedPixels[(static_cast<std::vector<Pixel>::size_type>(yPrime) * static_cast<std::vector<Pixel>::size_type>(newWidth)) + static_cast<std::vector<Pixel>::size_type>(xPrime)] = finalColor;
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
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: " + filename);
  }

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
float colorDistance(const Pixel& pixel1, const Pixel& pixel2) {
    return std::sqrt(
        static_cast<float>(std::pow(pixel1.red - pixel2.red, 2)) +
        static_cast<float>(std::pow(pixel1.green - pixel2.green, 2)) +
        static_cast<float>(std::pow(pixel1.blue - pixel2.blue, 2))
    );
}

// Función para eliminar colores menos frecuentes
std::vector<Pixel> removeLeastFrequentColors(const std::vector<Pixel>& pixels, int n) {
    if (n < 0) { throw std::invalid_argument("El número de colores a eliminar no puede ser negativo.");}

    std::map<Pixel, int, bool(*)(const Pixel&, const Pixel&)> colorFrequency([](const Pixel& pixela, const Pixel& pixelb) {
        return std::tie(pixela.red, pixela.green, pixela.blue) < std::tie(pixelb.red, pixelb.green, pixelb.blue);
    });

    for (const auto& pixel : pixels) { colorFrequency[pixel]++;}

    std::vector<std::pair<Pixel, int>> colorFreqVec(colorFrequency.begin(), colorFrequency.end());
    std::ranges::sort(colorFreqVec, [](const auto& colorFreqa, const auto& colorFreqb) {
        return colorFreqa.second < colorFreqb.second ||
               (colorFreqa.second == colorFreqb.second && (colorFreqa.first.blue > colorFreqb.first.blue ||
               (colorFreqa.first.blue == colorFreqb.first.blue && (colorFreqa.first.green > colorFreqb.first.green ||
               (colorFreqa.first.green == colorFreqb.first.green && colorFreqa.first.red > colorFreqb.first.red)))));
    });
    std::set<Pixel> colorsToKeep;
    for (auto i = static_cast<size_t>(n); i < colorFreqVec.size(); ++i) { // Conversión segura
        colorsToKeep.insert(colorFreqVec[i].first);
    }
    std::map<Pixel, Pixel> colorReplacement;
    for (size_t i = 0; i < static_cast<size_t>(n) && i < colorFreqVec.size(); ++i) { // Conversión segura
        const Pixel colorToRemove = colorFreqVec[i].first;
        float minDistance = std::numeric_limits<float>::max();
        Pixel closestColor = {.red = 0, .green = 0, .blue = 0};

        for (const auto& color : colorsToKeep) {
            const float distance = colorDistance(colorToRemove, color);
            if (distance < minDistance) {
                minDistance = distance;
                closestColor = color;
            }
        }
        colorReplacement[colorToRemove] = closestColor;
    }
    std::vector<Pixel> modifiedPixels = pixels;
    for (auto& pixel : modifiedPixels) {
      if (colorReplacement.contains(pixel)) { pixel = colorReplacement[pixel]; }
    }
    return modifiedPixels;
}