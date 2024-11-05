#include <iostream>
#include "../common/progargs.hpp"

int main(int argc, char* argv[]) {
    try {
        // Procesar los argumentos de la línea de comandos
        ProgramArgs args = processArgs(argc, argv);

        // Ejecutar la operación solicitada
        if (args.operation == "info") {
            std::cout << "Operación: info\nArchivo de entrada: " << args.inputFile
                      << "\nArchivo de salida: " << args.outputFile << '\n';
        } else if (args.operation == "maxlevel") {
            int maxLevel = std::stoi(args.extraParams[0]);
            std::cout << "Operación: maxlevel\nNuevo valor: " << maxLevel << '\n';
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

