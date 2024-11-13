#include "../common/misc.hpp"
#include "../common/progargs.hpp"
#include "../imgaos/imageaos.hpp"

#include <iostream>
#include <stdexcept>



namespace {
  constexpr int ocho = 8;
  constexpr int diceciseis = 16;
  void printInfo(const std::string& inputFilename, const PPMMetadata& metadata) {
    std::cout << "Operación: info\n"
              << "Archivo de entrada: " << inputFilename << "\n"
              << "Número mágico: " << metadata.magicNumber << "\n"
              << "Ancho: " << metadata.width << "\n"
              << "Alto: " << metadata.height << "\n"
              << "Valor máximo de color: " << metadata.maxColorValue << '\n';
  }

  void handleMaxLevel(std::vector<Pixel>& pixels, const std::vector<std::string>& arguments, const PPMMetadata& metadata, const std::string& outputFilename) {
    const int maxLevel = std::stoi(arguments[4]);
    std::cout << "Operación: maxlevel\nNuevo valor: " << maxLevel << '\n';
    if (numberInXbitRange(maxLevel) == ocho) {
      scaleIntensity8bit(pixels, metadata.maxColorValue, maxLevel);
    } else if (numberInXbitRange(maxLevel) == diceciseis) {
      scaleIntensity16bit(pixels, metadata.maxColorValue, maxLevel);
    } else {
      throw std::invalid_argument("MaxLevel no valido");
    }
    const PPMMetadata newMetadata = {.magicNumber = metadata.magicNumber, .width = metadata.width, .height = metadata.height, .maxColorValue = maxLevel};
    saveImage(outputFilename, pixels, newMetadata);
  }

  void handleResize(std::vector<Pixel>& pixels, const std::vector<std::string>& arguments, const PPMMetadata& metadata, const std::string& outputFilename) {
    const int newWidth = std::stoi(arguments[4]);
    const int newHeight = std::stoi(arguments[5]);
    std::cout << "Operación: resize\nAncho nuevo: " << newWidth
              << "\nAlto nuevo: " << newHeight << '\n';
    PPMMetadata const newMetadata = {.magicNumber = metadata.magicNumber, .width = newWidth, .height = newHeight, .maxColorValue = metadata.maxColorValue};
    pixels = resizeImage(pixels, metadata, newWidth, newHeight);
    saveImage(outputFilename, pixels, newMetadata);
  }

  void handleCutFreq(std::vector<Pixel>& pixels, const std::vector<std::string>& arguments, const PPMMetadata& metadata, const std::string& outputFilename) {
    const int numColorsToRemove = std::stoi(arguments[4]);
    std::cout << "Operación: cutfreq\nColores a eliminar: " << numColorsToRemove << '\n';
    pixels = removeLeastFrequentColors(pixels, numColorsToRemove);
    saveImage(outputFilename, pixels, metadata);
  }

  void handleCompress(const std::vector<Pixel>& pixels, const std::string& outputFilename, const PPMMetadata& metadata) {
    std::cout << "Operación: compress\n";
    saveImage(outputFilename, pixels, metadata);
  }
}

int main(const int argc, char* argv[]) {
    if (argc < 4) {
        printUsage();
        return 1;
    }
    // Convertir los parámetros de argv a std::vector<std::string>
    const std::vector<std::string> arguments(argv, argv + argc);
    const std::string& inputFilename = arguments[1];
    const std::string& outputFilename = arguments[2];
    const std::string& operation = arguments[3];

    try {
        const PPMMetadata metadata = getPPMMetadata(inputFilename);
        std::vector<Pixel> pixels = loadImage(inputFilename, metadata);

        if (operation == "info") {
            printInfo(inputFilename, metadata);
        } else if (operation == "maxlevel") {
            if (constexpr int cinco = 5; argc != cinco) {printUsage();}
            handleMaxLevel(pixels, arguments, metadata, outputFilename);
        } else if (operation == "resize") {
            if (constexpr int seis = 6; argc != seis) {printUsage();}
            handleResize(pixels, arguments, metadata, outputFilename);
        } else if (operation == "cutfreq") {
            if (constexpr int cinco = 5; argc != cinco) {printUsage();}
            handleCutFreq(pixels, arguments, metadata, outputFilename);
        } else if (operation == "compress") {
            handleCompress(pixels, outputFilename, metadata);
        } else {
            printUsage();
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}