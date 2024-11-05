//
// Created by diego on 20/10/24.
//

#include "progargs.hpp"
#include <iostream>
#include <stdexcept>
#include <cstdlib>  // Para std::stoi

// Función auxiliar para validar la operación y número de argumentos
void validateOperation(const std::string& operation, int argc);

// Funciones auxiliares para validar cada operación
void validateMaxlevel(const std::vector<std::string>& args);
void validateResize(const std::vector<std::string>& args);
void validateCutfreq(const std::vector<std::string>& args);

ProgramArgs processArgs(int argc, char* argv[]) {
    if (argc < 4) {
        printUsage();
        throw std::runtime_error("Error: Invalid number of arguments.");
    }

    ProgramArgs args;
    args.inputFile = argv[1];
    args.outputFile = argv[2];
    args.operation = argv[3];

    validateOperation(args.operation, argc);

    // Validar parámetros adicionales según la operación
    if (args.operation == "maxlevel") {
        args.extraParams.push_back(argv[4]);
        validateMaxlevel(args.extraParams);
    } else if (args.operation == "resize") {
        args.extraParams.push_back(argv[4]);
        args.extraParams.push_back(argv[5]);
        validateResize(args.extraParams);
    } else if (args.operation == "cutfreq") {
        args.extraParams.push_back(argv[4]);
        validateCutfreq(args.extraParams);
    }

    return args;
}

void validateOperation(const std::string& operation, int argc) {
    if (operation != "info" && operation != "maxlevel" &&
        operation != "resize" && operation != "cutfreq" &&
        operation != "compress") {
        throw std::runtime_error("Error: Invalid option: " + operation);
    }

    if ((operation == "info" || operation == "compress") && argc != 4) {
        throw std::runtime_error("Error: Invalid extra arguments for " + operation + ".");
    } else if (operation == "maxlevel" && argc != 5) {
        throw std::runtime_error("Error: Invalid number of extra arguments for maxlevel.");
    } else if (operation == "resize" && argc != 6) {
        throw std::runtime_error("Error: Invalid number of extra arguments for resize.");
    } else if (operation == "cutfreq" && argc != 5) {
        throw std::runtime_error("Error: Invalid number of extra arguments for cutfreq.");
    }
}

void validateMaxlevel(const std::vector<std::string>& args) {
    int maxLevel = std::stoi(args[0]);
    int constexpr maxlength = 65535;
    if (maxLevel < 0 || maxLevel > maxlength) {
        throw std::runtime_error("Error: Invalid maxlevel: " + std::to_string(maxLevel));
    }
}

void validateResize(const std::vector<std::string>& args) {
    int newWidth = std::stoi(args[0]);
    int newHeight = std::stoi(args[1]);
    if (newWidth <= 0) {
        throw std::runtime_error("Error: Invalid resize width: " + std::to_string(newWidth));
    }
    if (newHeight <= 0) {
        throw std::runtime_error("Error: Invalid resize height: " + std::to_string(newHeight));
    }
}

void validateCutfreq(const std::vector<std::string>& args) {
    int n = std::stoi(args[0]);
    if (n <= 0) {
        throw std::runtime_error("Error: Invalid cutfreq: " + std::to_string(n));
    }
}

void printUsage() {
    std::cerr << "Uso: imtool <input_file> <output_file> <operación> [opciones adicionales]\n"
              << "Operaciones disponibles:\n"
              << "  info: Muestra metadatos de la imagen.\n"
              << "  maxlevel <nuevo_valor>: Escala la intensidad a un nuevo valor máximo (0-65535).\n"
              << "  resize <nuevo_ancho> <nuevo_alto>: Redimensiona la imagen a un tamaño nuevo (positivo).\n"
              << "  cutfreq <n>: Elimina los n colores menos frecuentes (n positivo).\n"
              << "  compress: Comprime la imagen al formato CPPM.\n";
}

