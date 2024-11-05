#include <iostream>
#include "../common/progargs.hpp"
#include "../imgaos/imageaos.hpp"

int main(int argc, char* argv[]) {
  try {
    // Procesar los argumentos de la línea de comandos
    ProgramArgs args = processArgs(argc, argv);

    // Ejecutar la operación solicitada
    if (args.operation == "info") {
      // Obtener y mostrar los metadatos del archivo PPM
      PPMMetadata metadata = getPPMMetadata(args.inputFile);
      std::cout << "Operación: info\n"
                << "Archivo de entrada: " << args.inputFile << "\n"
                << "Número mágico: " << metadata.magicNumber << "\n"
                << "Ancho: " << metadata.width << "\n"
                << "Alto: " << metadata.height << "\n"
                << "Valor máximo de color: " << metadata.maxColorValue << '\n';

    } else if (args.operation == "maxlevel") {
      // Escalar los niveles de intensidad de la imagen
      int maxLevel = std::stoi(args.extraParams[0]);
      std::cout << "Operación: maxlevel\nNuevo valor: " << maxLevel << '\n';

      // Obtener los metadatos y los píxeles de la imagen
      PPMMetadata metadata = getPPMMetadata(args.inputFile);
      std::vector<Pixel> pixels = loadImage(args.inputFile, metadata.width, metadata.height, metadata.maxColorValue);

      // Escalar la intensidad al nuevo valor máximo
      scaleIntensity(pixels, metadata.maxColorValue, maxLevel);

      // Guardar la imagen escalada en el archivo de salida
      saveImage(args.outputFile, pixels, metadata.width, metadata.height, maxLevel);

    } else if (args.operation == "resize") {
      int newWidth = std::stoi(args.extraParams[0]);
      int newHeight = std::stoi(args.extraParams[1]);
      std::cout << "Operación: resize\nAncho nuevo: " << newWidth
                << "\nAlto nuevo: " << newHeight << '\n';

    } else if (args.operation == "cutfreq") {
      int n = std::stoi(args.extraParams[0]);
      std::cout << "Operación: cutfreq\nColores a eliminar: " << n << '\n';

    } else if (args.operation == "compress") {
      std::cout << "Operación: compress\n";
    }

  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return -1;  // Devolver código de error
  }

  return 0;
}
