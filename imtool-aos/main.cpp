#include <iostream>
#include <stdexcept>
#include "../common/progargs.hpp"
#include "../imgaos/imageaos.hpp"

namespace {
  void printInfo(const std::string& inputFilename, const PPMMetadata& metadata) {
    std::cout << "Operación: info\n"
              << "Archivo de entrada: " << inputFilename << "\n"
              << "Número mágico: " << metadata.magicNumber << "\n"
              << "Ancho: " << metadata.width << "\n"
              << "Alto: " << metadata.height << "\n"
              << "Valor máximo de color: " << metadata.maxColorValue << '\n';
  }

  void handleMaxLevel(std::vector<Pixel>& pixels, const ProgramArgs& args, const PPMMetadata& metadata) {
    const int maxLevel = std::stoi(args.extraParams[0]);
    std::cout << "Operación: maxlevel\nNuevo valor: " << maxLevel << '\n';
    scaleIntensity(pixels, metadata.maxColorValue, maxLevel);
    const PPMMetadata newMetadata = {.magicNumber = metadata.magicNumber, .width = metadata.width, .height = metadata.height, .maxColorValue = maxLevel};
    saveImage(args.outputFile, pixels, newMetadata);
  }

  void handleResize(std::vector<Pixel>& pixels, const ProgramArgs& args, const PPMMetadata& metadata) {
    const int newWidth = std::stoi(args.extraParams[0]);
    const int newHeight = std::stoi(args.extraParams[1]);
    std::cout << "Operación: resize\nAncho nuevo: " << newWidth
              << "\nAlto nuevo: " << newHeight << '\n';
    PPMMetadata const newMetadata = {.magicNumber = metadata.magicNumber, .width = newWidth, .height = newHeight, .maxColorValue = metadata.maxColorValue};
    pixels = resizeImage(pixels, metadata, newWidth, newHeight);
    saveImage(args.outputFile, pixels, newMetadata);
  }

  void handleCutFreq(std::vector<Pixel>& pixels, const ProgramArgs& args, const PPMMetadata& metadata) {
    const int numColorsToRemove = std::stoi(args.extraParams[0]);
    std::cout << "Operación: cutfreq\nColores a eliminar: " << numColorsToRemove << '\n';
    pixels = removeLeastFrequentColors(pixels, numColorsToRemove);
    saveImage(args.outputFile, pixels, metadata);
  }

  void handleCompress(const std::vector<Pixel>& pixels, const ProgramArgs& args, const PPMMetadata& metadata) {
    std::cout << "Operación: compress\n";
    saveImage(args.outputFile, pixels, metadata);
  }
}

int main(const int argc, char* argv[]) {
    try {
        // Convertir los parámetros de argv a std::vector<std::string>
        const std::vector<std::string> arguments(argv, argv + argc);

        // Procesar y validar argumentos usando processArgs
        const ProgramArgs args = processArgs(arguments);

        // Cargar metadatos e imagen
        const PPMMetadata metadata = getPPMMetadata(args.inputFile);
        std::vector<Pixel> pixels = loadImage(args.inputFile, metadata);

        // Ejecutar operación según el tipo en args.operation
        if (args.operation == "info") {
            printInfo(args.inputFile, metadata);
        } else if (args.operation == "maxlevel") {
            handleMaxLevel(pixels, args, metadata);
        } else if (args.operation == "resize") {
            handleResize(pixels, args, metadata);
        } else if (args.operation == "cutfreq") {
            handleCutFreq(pixels, args, metadata);
        } else if (args.operation == "compress") {
            handleCompress(pixels, args, metadata);
        } else {
            std::cerr << "Error: Operación desconocida.\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}
