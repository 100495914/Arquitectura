#include "progargs.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>

ProgramArgs processArgs(const std::vector<std::string>& arguments) {
    if (arguments.size() < 4) {
        throw std::runtime_error("Error: Invalid number of arguments: " + std::to_string(arguments.size() - 1));
    }

    ProgramArgs args;
    args.inputFile = arguments[1];
    args.outputFile = arguments[2];
    args.operation = arguments[3];

    validateOperation(args.operation, static_cast<int>(arguments.size()));
    constexpr int cinco = 5;
    constexpr int seis = 6;
    // Validar parámetros adicionales según la operación
    if (args.operation == "maxlevel") {
        if (arguments.size() != cinco) {
            throw std::runtime_error("Error: Invalid number of extra arguments for maxlevel: " + std::to_string(arguments.size() - 4));
        }
        args.extraParams.push_back(arguments[4]);
        validateMaxlevel(args.extraParams);
    } else if (args.operation == "resize") {
        if (arguments.size() != seis) {
            throw std::runtime_error("Error: Invalid number of extra arguments for resize: " + std::to_string(arguments.size() - 4));
        }
        args.extraParams.push_back(arguments[4]);
        args.extraParams.push_back(arguments[cinco]);
        validateResize(args.extraParams);
    } else if (args.operation == "cutfreq") {
        if (arguments.size() != cinco) {
            throw std::runtime_error("Error: Invalid number of extra arguments for cutfreq: " + std::to_string(arguments.size() - 4));
        }
        args.extraParams.push_back(arguments[4]);
        validateCutfreq(args.extraParams);
    }

    return args;
}

void validateOperation(const std::string& operation, const int argc) {
    constexpr int cinco = 5;
    constexpr int seis = 6;
    if (operation != "info" && operation != "maxlevel" &&
        operation != "resize" && operation != "cutfreq" &&
        operation != "compress") {
        throw std::runtime_error("Error: Invalid option: " + operation);
    }

    if ((operation == "info" || operation == "compress") && argc != 4) {
        throw std::runtime_error("Error: Invalid extra arguments for " + operation + ": " + std::to_string(argc - 4));
    }
    if (operation == "maxlevel" && argc != cinco) {
        throw std::runtime_error("Error: Invalid number of extra arguments for maxlevel: " + std::to_string(argc - 4));
    }
    if (operation == "resize" && argc != seis) {
        throw std::runtime_error("Error: Invalid number of extra arguments for resize: " + std::to_string(argc - 4));
    }
    if (operation == "cutfreq" && argc != cinco) {
        throw std::runtime_error("Error: Invalid number of extra arguments for cutfreq: " + std::to_string(argc - 4));
    }
}

void validateMaxlevel(const std::vector<std::string>& args) {
    constexpr int maxValue = 65535;
    try {
        const int maxLevel = std::stoi(args[0]);
        if (maxLevel < 0 || maxLevel > maxValue) {
            throw std::runtime_error("Error: Invalid maxlevel: " + std::to_string(maxLevel));
        }
    } catch (const std::invalid_argument&) {
        throw std::runtime_error("Error: Invalid maxlevel: " + args[0]);
    }
}

void validateResize(const std::vector<std::string>& args) {
    try {
        const int newWidth = std::stoi(args[0]);
        const int newHeight = std::stoi(args[1]);
        if (newWidth <= 0) {
            throw std::runtime_error("Error: Invalid resize width: " + std::to_string(newWidth));
        }
        if (newHeight <= 0) {
            throw std::runtime_error("Error: Invalid resize height: " + std::to_string(newHeight));
        }
    } catch (const std::invalid_argument&) {
        throw std::runtime_error("Error: Invalid resize height or width: " + args[0] + " or " + args[1]);
    }
}

void validateCutfreq(const std::vector<std::string>& args) {
    try {
        const int numColorsToRemove = std::stoi(args[0]);
        if (numColorsToRemove <= 0) {
            throw std::runtime_error("Error: Invalid cutfreq: " + std::to_string(numColorsToRemove));
        }
    } catch (const std::invalid_argument&) {
        throw std::runtime_error("Error: Invalid cutfreq: " + args[0]);
    }
}

