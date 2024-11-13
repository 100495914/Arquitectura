//
// Created by Alberto on 13/11/2024.
//

#include "imtool_soa_aux.hpp"

#include "imgsoa/imagesoa.cpp"

#include <iostream>
#include <optional>

namespace {
  constexpr int cinco = 5;
  constexpr int ocho      = 8;
  constexpr int dieciseis = 16;

  int operationCode(std::string const & operation) {
    if (operation == "info") { return 0; }
    if (operation == "maxlevel") { return 1; }
    if (operation == "resize") { return 2; }
    if (operation == "cutfreq") { return 3; }
    if (operation == "compress") { return 4; }
    return -1;
  }
}  // namespace

int checkProperArgumentNumber(int const operation, size_t argv_size) {
  argv_size -= 1;
  if (operation == 0 && argv_size != 3) {
    std::cerr << "Error: Info requiere 3 argumentos.\n";
    return 0;
  }
  if (operation == 1 && argv_size != 4) {
    std::cerr << "Error: MaxLevel requiere 4 argumentos.\n";
    return 0;
  }

  if (operation == 2 && argv_size != cinco) {
    std::cerr << "Error: Resize requiere 4 argumentos.\n";
    return 0;
  }
  if (operation == 3 && argv_size != 4) {
    std::cerr << "Error: CutFreq requiere 4 argumentos.\n";
    return 0;
  }
  if (operation == 4 && argv_size != 3) {
    std::cerr << "Error: Compress requiere 3 argumentos.\n";
    return 0;
  }
  return 1;
}

auto sanitizeArgs(std::vector<std::string> const & args) -> std::optional<Command> {
  // Ensure that at least the minimum required arguments are provided
  if (args.size() < 4) { throw std::runtime_error("Se requieren 4 argumentos como minimo"); }

  try {
    Command cmd = {
      .input     = args[1],                // input file
      .output    = args[2],                // output file
      .operation = operationCode(args[3])  // operation
    };

    // Parse optional integer arguments
    if (args.size() > 4) { cmd.op1 = std::stoi(args[4]); }
    if (args.size() > cinco) { cmd.op2 = std::stoi(args[cinco]); }

    return cmd;  // Return the successfully parsed command

  } catch (std::invalid_argument &) {
    throw std::runtime_error("Formato incorrecto en los argumentos");
  } catch (std::out_of_range &) {
    throw std::runtime_error("Entero fuera de rango en los argumentos");
  }
}

namespace {
  void hlpr_handleMaxLevel8bit(PPMMetadata const & metadata, Command const & cmd) {
    std::string const input  = cmd.input;
    std::string const output = cmd.output;
    uint const newMax        = static_cast<uint>(cmd.op1);

    int const newMaxBitType = numberInXbitRange(newMax);
    // Create and load 8-bit image
    auto image8 = std::make_unique<ImageSOA_8bit>(metadata);
    image8->loadData(input);

    // Handle scaling based on new max range
    if (newMaxBitType == ocho) {
      image8->scaleIntensity(newMax);
      image8->saveToFile(output);
    } else if (newMaxBitType == dieciseis) {
      // Scale to 16-bit and save
      auto image8_16 = image8->scaleIntensityChannelSizeChange(newMax);
      image8_16->saveToFile(output);
    } else {
      std::cerr << "Rango de newMax no valido.\n";
    }
  }

  void hlpr_handleMaxLevel16bit(PPMMetadata const & metadata, Command const & cmd) {
    std::string const input  = cmd.input;
    std::string const output = cmd.output;
    uint const newMax        = static_cast<uint>(cmd.op1);

    int const newMaxBitType = numberInXbitRange(newMax);
    // Create and load 8-bit image
    auto const image16 = std::make_unique<ImageSOA_16bit>(metadata);
    image16->loadData(input);

    // Handle scaling based on new max range
    if (newMaxBitType == dieciseis) {
      image16->scaleIntensity(newMax);
      image16->saveToFile(output);
    } else if (newMaxBitType == ocho) {
      // Scale to 8-bit and save
      auto const image16_8 = image16->scaleIntensityChannelSizeChange(newMax);
      image16_8->saveToFile(output);
    } else {
      std::cerr << "Rango de newMax no valido.\n";
    }
  }
}  // namespace

void handleMaxLevel(Command const & cmd) {
  try {
    std::string const input = cmd.input;

    switch (PPMMetadata const metadata = loadMetadata(input);
            numberInXbitRange(metadata.maxColorValue)) {
      case ocho:
        hlpr_handleMaxLevel8bit(metadata, cmd);
        break;
      case dieciseis:
        hlpr_handleMaxLevel16bit(metadata, cmd);
        break;
      default:
        std::cerr << "Unsupported image bit type.\n";
        break;
    }

  } catch (std::exception const & e) { std::cerr << "Error: " << e.what() << '\n'; }
}
