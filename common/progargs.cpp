#include "progargs.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>

ProgramArgs processArgs(const std::vector<std::string>& arguments) {
    if (arguments.size() < 4) {
        printUsage();
        throw std::runtime_error("Error: Invalid number of arguments.");
    }

    ProgramArgs args;
    args.inputFile = arguments[1];
    args.outputFile = arguments[2];
    args.operation = arguments[3];

    validateOperation(args.operation, static_cast<int>(arguments.size()));

    // Validar parámetros adicionales según la operación
    if (args.operation == "maxlevel") {
        args.extraParams.push_back(arguments[4]);
        validateMaxlevel(args.extraParams);
    } else if (args.operation == "resize") {
        args.extraParams.push_back(arguments[4]);
        constexpr int cinco =5;
        args.extraParams.push_back(arguments[cinco]);
        validateResize(args.extraParams);
    } else if (args.operation == "cutfreq") {
        args.extraParams.push_back(arguments[4]);
        validateCutfreq(args.extraParams);
    }

    return args;
}

void validateOperation(const std::string& operation, const int argc) {
    if (operation != "info" && operation != "maxlevel" &&
        operation != "resize" && operation != "cutfreq" &&
        operation != "compress") {
        throw std::runtime_error("Error: Invalid option: " + operation);
    }

    if ((operation == "info" || operation == "compress") && argc != 4) {
        throw std::runtime_error("Error: Invalid extra arguments for " + operation + ".");
    }
    if (constexpr int cinco = 5; operation == "maxlevel" && argc != cinco) {
        throw std::runtime_error("Error: Invalid number of extra arguments for maxlevel.");
    }
    if (constexpr int seis = 6; operation == "resize" && argc != seis) {
        throw std::runtime_error("Error: Invalid number of extra arguments for resize.");
    }
    if (constexpr int cinco = 5; operation == "cutfreq" && argc != cinco) {
        throw std::runtime_error("Error: Invalid number of extra arguments for cutfreq.");
    }
}

void validateMaxlevel(const std::vector<std::string>& args) {
    const int maxLevel = std::stoi(args[0]);
    if (constexpr int maxlength = 65535;maxLevel < 0 || maxLevel > maxlength) {
        throw std::runtime_error("Error: Invalid maxlevel: " + std::to_string(maxLevel));
    }
}

void validateResize(const std::vector<std::string>& args) {
    const int newWidth = std::stoi(args[0]);
    const int newHeight = std::stoi(args[1]);
    if (newWidth <= 0) {
        throw std::runtime_error("Error: Invalid resize width: " + std::to_string(newWidth));
    }
    if (newHeight <= 0) {
        throw std::runtime_error("Error: Invalid resize height: " + std::to_string(newHeight));
    }
}

void validateCutfreq(const std::vector<std::string>& args) {
    const int numColorsToRemove = std::stoi(args[0]);
    if (numColorsToRemove <= 0) {
        throw std::runtime_error("Error: Invalid cutfreq: " + std::to_string(numColorsToRemove));
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
