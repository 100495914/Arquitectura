#include <iostream>
#include <stdexcept>
#include "../imgaos/imageaos.hpp"

void printUsage() {
    std::cout << "Uso: ./imtool-aos <input.ppm> <output.ppm> <operation> [params...]\n";
    std::cout << "Operaciones disponibles:\n";
    std::cout << "  resize <newWidth> <newHeight>\n";
    std::cout << "  cutfreq <n>\n";  // Operación actualizada
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printUsage();
        return 1;
    }

    std::string inputFilename = argv[1];
    std::string outputFilename = argv[2];
    std::string operation = argv[3];

    try {
        PPMMetadata metadata = getPPMMetadata(inputFilename);
        std::vector<Pixel> pixels = loadImage(inputFilename, metadata.width, metadata.height, metadata.maxColorValue);

        if (operation == "resize") {
            if (argc != 6) {
                printUsage();
                return 1;
            }
            int newWidth = std::stoi(argv[4]);
            int newHeight = std::stoi(argv[5]);
            pixels = resizeImage(pixels, metadata.width, metadata.height, newWidth, newHeight);
            saveImage(outputFilename, pixels, newWidth, newHeight, metadata.maxColorValue);
        } else if (operation == "cutfreq") {
            if (argc != 5) {
                std::cerr << "Error: Invalid number of extra arguments for cutfreq: " << argc - 4 << std::endl;
                return -1;
            }
            int n = std::stoi(argv[4]);
            if (n <= 0) {
                std::cerr << "Error: Invalid cutfreq: " << n << std::endl;
                return -1;
            }
            pixels = removeLeastFrequentColors(pixels, n); // Implementar la lógica de cutfreq
            saveImage(outputFilename, pixels, metadata.width, metadata.height, metadata.maxColorValue);
        } else {
            printUsage();
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
