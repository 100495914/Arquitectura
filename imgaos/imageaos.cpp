#include "imageaos.hpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cmath>
#include <map>
#include <algorithm>
#include <set>

// Implementación de la función interpolate
Pixel interpolate(const Pixel& p1, const Pixel& p2, float weight) {
    Pixel result;
    result.red = static_cast<uint8_t>(std::round(static_cast<float>(p1.red) + weight * (static_cast<float>(p2.red) - static_cast<float>(p1.red))));
    result.green = static_cast<uint8_t>(std::round(static_cast<float>(p1.green) + weight * (static_cast<float>(p2.green) - static_cast<float>(p1.green))));
    result.blue = static_cast<uint8_t>(std::round(static_cast<float>(p1.blue) + weight * (static_cast<float>(p2.blue) - static_cast<float>(p1.blue))));
    return result;
}

// Implementación de la función para redimensionar la imagen
std::vector<Pixel> resizeImage(const std::vector<Pixel>& originalPixels, int originalWidth, int originalHeight, int newWidth, int newHeight) {
    std::vector<Pixel> resizedPixels(static_cast<std::vector<Pixel>::size_type>(newWidth) * static_cast<std::vector<Pixel>::size_type>(newHeight));

    for (int yPrime = 0; yPrime < newHeight; ++yPrime) {
        for (int xPrime = 0; xPrime < newWidth; ++xPrime) {
            float x = (static_cast<float>(xPrime) * static_cast<float>(originalWidth)) / static_cast<float>(newWidth);
            float y = (static_cast<float>(yPrime) * static_cast<float>(originalHeight)) / static_cast<float>(newHeight);

            int xl = static_cast<int>(std::floor(x));
            int xh = static_cast<int>(std::ceil(x));
            int yl = static_cast<int>(std::floor(y));
            int yh = static_cast<int>(std::ceil(y));

            if (xh >= originalWidth) xh = originalWidth - 1;
            if (yh >= originalHeight) yh = originalHeight - 1;

            Pixel p1 = originalPixels[static_cast<std::vector<Pixel>::size_type>(yl) * static_cast<std::vector<Pixel>::size_type>(originalWidth) + static_cast<std::vector<Pixel>::size_type>(xl)];
            Pixel p2 = originalPixels[static_cast<std::vector<Pixel>::size_type>(yl) * static_cast<std::vector<Pixel>::size_type>(originalWidth) + static_cast<std::vector<Pixel>::size_type>(xh)];
            Pixel p3 = originalPixels[static_cast<std::vector<Pixel>::size_type>(yh) * static_cast<std::vector<Pixel>::size_type>(originalWidth) + static_cast<std::vector<Pixel>::size_type>(xl)];
            Pixel p4 = originalPixels[static_cast<std::vector<Pixel>::size_type>(yh) * static_cast<std::vector<Pixel>::size_type>(originalWidth) + static_cast<std::vector<Pixel>::size_type>(xh)];

            float xWeight = x - static_cast<float>(xl);
            float yWeight = y - static_cast<float>(yl);

            Pixel c1 = interpolate(p1, p2, xWeight);
            Pixel c2 = interpolate(p3, p4, xWeight);
            Pixel finalColor = interpolate(c1, c2, yWeight);

            resizedPixels[static_cast<std::vector<Pixel>::size_type>(yPrime) * static_cast<std::vector<Pixel>::size_type>(newWidth) + static_cast<std::vector<Pixel>::size_type>(xPrime)] = finalColor;
        }
    }

    return resizedPixels;
}

// Implementación de la función para guardar la imagen
void saveImage(const std::string& filename, const std::vector<Pixel>& pixels, int width, int height, int maxColorValue) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    file << "P6\n" << width << " " << height << "\n" << maxColorValue << "\n";
    int bytesPerChannel = (maxColorValue < 256) ? 1 : 2;

    for (const auto& pixel : pixels) {
        if (bytesPerChannel == 1) {
            unsigned char r = static_cast<unsigned char>(pixel.red);
            unsigned char g = static_cast<unsigned char>(pixel.green);
            unsigned char b = static_cast<unsigned char>(pixel.blue);
            file.write(reinterpret_cast<char*>(&r), 1);
            file.write(reinterpret_cast<char*>(&g), 1);
            file.write(reinterpret_cast<char*>(&b), 1);
        } else {
            unsigned short r = static_cast<unsigned short>(pixel.red);
            unsigned short g = static_cast<unsigned short>(pixel.green);
            unsigned short b = static_cast<unsigned short>(pixel.blue);
            file.write(reinterpret_cast<char*>(&r), 2);
            file.write(reinterpret_cast<char*>(&g), 2);
            file.write(reinterpret_cast<char*>(&b), 2);
        }
    }

    file.close();
}

// Implementación de la función para cargar la imagen
std::vector<Pixel> loadImage(const std::string& filename, int width, int height, int maxColorValue) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    std::string magicNumber;
    file >> magicNumber;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::vector<Pixel> pixels(static_cast<std::vector<Pixel>::size_type>(width) * static_cast<std::vector<Pixel>::size_type>(height));
    int bytesPerChannel = (maxColorValue < 256) ? 1 : 2;

    for (auto& pixel : pixels) {
        if (bytesPerChannel == 1) {
            unsigned char r, g, b;
            file.read(reinterpret_cast<char*>(&r), 1);
            file.read(reinterpret_cast<char*>(&g), 1);
            file.read(reinterpret_cast<char*>(&b), 1);
            pixel.red = r;
            pixel.green = g;
            pixel.blue = b;
        } else {
            unsigned short r, g, b;
            file.read(reinterpret_cast<char*>(&r), 2);
            file.read(reinterpret_cast<char*>(&g), 2);
            file.read(reinterpret_cast<char*>(&b), 2);
            pixel.red = static_cast<uint8_t>(r); // Conversión segura
            pixel.green = static_cast<uint8_t>(g); // Conversión segura
            pixel.blue = static_cast<uint8_t>(b); // Conversión segura
        }
    }

    file.close();
    return pixels;
}

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
float colorDistance(const Pixel& a, const Pixel& b) {
    return std::sqrt(
        static_cast<float>(std::pow(a.red - b.red, 2)) +
        static_cast<float>(std::pow(a.green - b.green, 2)) +
        static_cast<float>(std::pow(a.blue - b.blue, 2))
    );
}

// Función para eliminar colores menos frecuentes
std::vector<Pixel> removeLeastFrequentColors(const std::vector<Pixel>& pixels, int n) {
    // Asegurarse de que n no sea negativo
    if (n < 0) {
        throw std::invalid_argument("El número de colores a eliminar no puede ser negativo.");
    }

    std::map<Pixel, int, bool(*)(const Pixel&, const Pixel&)> colorFrequency([](const Pixel& a, const Pixel& b) {
        return std::tie(a.red, a.green, a.blue) < std::tie(b.red, b.green, b.blue);
    });

    for (const auto& pixel : pixels) {
        colorFrequency[pixel]++;
    }

    std::vector<std::pair<Pixel, int>> colorFreqVec(colorFrequency.begin(), colorFrequency.end());
    std::sort(colorFreqVec.begin(), colorFreqVec.end(), [](const auto& a, const auto& b) {
        return a.second < b.second ||
               (a.second == b.second && (a.first.blue > b.first.blue ||
               (a.first.blue == b.first.blue && (a.first.green > b.first.green ||
               (a.first.green == b.first.green && a.first.red > b.first.red)))));
    });

    std::set<Pixel> colorsToKeep;
    for (auto i = static_cast<size_t>(n); i < colorFreqVec.size(); ++i) { // Conversión segura
        colorsToKeep.insert(colorFreqVec[i].first);
    }

    std::map<Pixel, Pixel> colorReplacement;
    for (size_t i = 0; i < static_cast<size_t>(n) && i < colorFreqVec.size(); ++i) { // Conversión segura
        Pixel colorToRemove = colorFreqVec[i].first;
        float minDistance = std::numeric_limits<float>::max();
        Pixel closestColor;

        for (const auto& color : colorsToKeep) {
            float distance = colorDistance(colorToRemove, color);
            if (distance < minDistance) {
                minDistance = distance;
                closestColor = color;
            }
        }
        colorReplacement[colorToRemove] = closestColor;
    }

    std::vector<Pixel> modifiedPixels = pixels;
    for (auto& pixel : modifiedPixels) {
        if (colorReplacement.count(pixel) > 0) {
            pixel = colorReplacement[pixel];
        }
    }

    return modifiedPixels;
}
