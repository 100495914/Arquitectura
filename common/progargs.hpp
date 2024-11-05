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
ProgramArgs processArgs(int argc, char* argv[]);

// Función para mostrar los mensajes de error
void printUsage();

#endif // PROGARGS_HPP


