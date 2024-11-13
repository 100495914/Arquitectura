//
// Created by diego on 20/10/24.
//

#ifndef PROGARGS_HPP
#define PROGARGS_HPP

#include <string>
#include <vector>

struct ProgramArgs {
    std::string inputFile;
    std::string outputFile;
    std::string operation;
    std::vector<std::string> extraParams;
};

// Función para procesar y validar los argumentos
ProgramArgs processArgs(const std::vector<std::string>& arguments);

// Función para mostrar los mensajes de error
void printUsage();

// Funciones auxiliares para validar cada operación
void validateMaxlevel(const std::vector<std::string>& args);
void validateResize(const std::vector<std::string>& args);
void validateCutfreq(const std::vector<std::string>& args);

// Función auxiliar para validar la operación y número de argumentos
void validateOperation(const std::string& operation, int argc);

#endif // PROGARGS_HPP


