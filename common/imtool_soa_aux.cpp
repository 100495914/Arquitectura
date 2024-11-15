//
// Created by Alberto on 13/11/2024.
//

#include "imtool_soa_aux.hpp"
#include "imgsoa/imagesoa.hpp"

#include <iostream>
#include <optional>

namespace {
  constexpr int cinco = 5;

  int operationCode(std::string const & operation) {
    if (typeid(operation) != typeid(std::string)) {
      std::cerr << "Error: Invalid arguments" << '\n';
      return -1;
    }
    if (operation == "info") { return 0; }
    if (operation == "maxlevel") { return 1; }
    if (operation == "resize") { return 2; }
    if (operation == "cutfreq") { return 3; }
    if (operation == "compress") { return 4; }
    std::cerr << "Error: Invalid option: " << operation << '\n';
    return -1;
  }
}  // namespace

int checkProperArgumentNumber(int const operation, size_t argv_size, Command const & cmd) {
  argv_size -= 1;
  if (operation == 0 && argv_size != 2) {
    std::cerr << "Error:  Invalid extra arguments for info:  " << cmd.output << '\n';
    return 0;
  }
  if (operation == 1 && argv_size != 4) {
    std::cerr << "Error:  Invalid extra arguments for maxlevel:  " << cmd.op2 << '\n';
    return 0;
  }

  if (operation == 2 && argv_size != cinco) {
    std::cerr << "Error:  Invalid extra arguments for resize:  " << cmd.output << '\n';
    return 0;
  }
  if (operation == 3 && argv_size != 4) {
    std::cerr << "Error:  Invalid extra arguments for cutfreq:  " << cmd.output << '\n';
    return 0;
  }
  return 1;
}

auto sanitizeArgs(std::vector<std::string> const & args) -> std::optional<Command> {
  Command cmd{};
  if (args[2] == "info") {
    cmd.input     = args[1];
    cmd.output    = "";
    cmd.operation = 0;
  } else {
    cmd.input     = args[1];
    cmd.output    = args[2];
    if (args.size() > 3) {
      cmd.operation = operationCode(args[3]);
    } else {
      cmd.operation = -1;
    }
  }

  // Parse optional integer arguments
  if (args.size() > 4) { cmd.op1 = args[4]; }
  if (args.size() > cinco) { cmd.op2 = args[cinco]; }

  return cmd;  // Return the successfully parsed command
}

namespace {
  void hlpr_handleMaxLevel8bit(PPMMetadata const & metadata, Command const & cmd) {
    std::string const input  = cmd.input;
    std::string const output = cmd.output;
    uint newMax              = 0;
    try {
      newMax = static_cast<uint>(std::stoi(cmd.op1));
    } catch (std::invalid_argument &) {
      std::cerr << "Error: Invalid maxlevel: " << cmd.op1 << '\n';
    }

    int const newMaxBitType = numberInXbitRange(newMax);
    // Create and load 8-bit image
    auto const image8 = std::make_unique<ImageSOA_8bit>(metadata);
    image8->loadData(input);

    // Handle scaling based on new max range
    if (newMaxBitType == ocho) {
      image8->maxLevel(newMax);
      image8->saveToFile(output);
    } else if (newMaxBitType == dieciseis) {
      // Scale to 16-bit and save
      auto const image8_16 = image8->maxLevelChangeChannelSize(newMax);
      image8_16->saveToFile(output);
    } else {
      std::cerr << "Rango de newMax no valido.\n";
    }
  }

  void hlpr_handleMaxLevel16bit(PPMMetadata const & metadata, Command const & cmd) {
    std::string const input  = cmd.input;
    std::string const output = cmd.output;
    uint newMax              = 0;
    try {
      newMax = static_cast<uint>(std::stoi(cmd.op1));
    } catch (std::invalid_argument &) {
      std::cerr << "Error: Invalid maxlevel: " << cmd.op1 << '\n';
    }

    int const newMaxBitType = numberInXbitRange(newMax);
    // Create and load 8-bit image
    auto const image16 = std::make_unique<ImageSOA_16bit>(metadata);
    image16->loadData(input);

    // Handle scaling based on new max range
    if (newMaxBitType == dieciseis) {
      image16->maxLevel(newMax);
      image16->saveToFile(output);
    } else if (newMaxBitType == ocho) {
      // Scale to 8-bit and save
      auto const image16_8 = image16->maxLevelChangeChannelSize(newMax);
      image16_8->saveToFile(output);
    } else {
      std::cerr << "Rango de newMax no valido.\n";
    }
  }

  void hlpr_resize8(Command const & cmd, std::string const & input, PPMMetadata const & metadata,
                    Dimensions const dim) {
    auto const image8 = std::make_unique<ImageSOA_8bit>(metadata);
    image8->loadData(input);
    image8->resize(dim);
    image8->saveToFile(cmd.output);
  }

  void hlpr_resize16(Command const & cmd, std::string const & input, PPMMetadata const & metadata,
                     Dimensions const dim) {
    auto const image16 = std::make_unique<ImageSOA_16bit>(metadata);
    image16->loadData(input);
    image16->resize(dim);
    image16->saveToFile(cmd.output);
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

void handleResize(Command const & cmd) {
  try {
    std::string const input    = cmd.input;
    PPMMetadata const metadata = loadMetadata(input);
    size_t width               = 0;
    size_t height              = 0;
    try {
      width = static_cast<size_t>(std::stoi(cmd.op1));
    } catch (std::invalid_argument &) {
      std::cerr << "Error: Invalid resize width: " << cmd.op1 << '\n';
    }
    try {
      height = static_cast<size_t>(std::stoi(cmd.op2));
    } catch (std::invalid_argument &) {
      std::cerr << "Error: Invalid resize height: " << cmd.op2 << '\n';
    }

    Dimensions const dim = {.width = width, .height = height};
    switch (numberInXbitRange(metadata.maxColorValue)) {
      case ocho:
        {
          hlpr_resize8(cmd, input, metadata, dim);
          break;
        }
      case dieciseis:
        {
          hlpr_resize16(cmd, input, metadata, dim);
        }
        break;
      default:
        {
          std::cerr << "Unsupported image bit type.\n";
        }
        break;
    }

  } catch (std::exception const & e) { std::cerr << "Error: " << e.what() << '\n'; }
}

int handleCutfreq(Command const & cmd) {
  try {
    std::string const input    = cmd.input;
    PPMMetadata const metadata = loadMetadata(input);
    size_t ncolors             = 0;
    try {
      ncolors = static_cast<size_t>(std::stoi(cmd.op1));
    } catch (std::invalid_argument &) {
      std::cerr << "Error: Invalid cutfreq: " << cmd.op1 << '\n';
      return -1;
    }
    switch (numberInXbitRange(metadata.maxColorValue)) {
      case ocho:
        {
          auto const image8 = std::make_unique<ImageSOA_8bit>(metadata);
          image8->loadData(input);
          image8->reduceColors(ncolors);
          image8->saveToFile(cmd.output);
          break;
        }
      case dieciseis:
        {
          auto const image16 = std::make_unique<ImageSOA_16bit>(metadata);
          image16->loadData(input);
          image16->reduceColors(ncolors);
          image16->saveToFile(cmd.output);
        }
        break;
      default:
        {
          std::cerr << "Error: Unsupported image bit type.\n";
        }
        break;
    }
  } catch (std::exception const & e) {
    std::cerr << "Error: " << e.what() << '\n';
    return -1;
  }
  return 0;
}

void handleInfo(Command const & cmd) {
  std::string const input                                = cmd.input;
  auto const [magicNumber, width, height, maxColorValue] = loadMetadata(input);
  std::cout << "Magic Number: " << magicNumber << '\n';
  std::cout << "Max Color Value: " << maxColorValue << '\n';
  std::cout << "Image height: " << height << '\n';
  std::cout << "Image width: " << width << '\n';
}

int operate(std::vector<std::string> const& arguments, std::optional<Command> const& cmd) {
  // Check if cmd has a value
  if (!cmd.has_value()) {
    std::cerr << "Error: Command is not provided.\n";
    return -1;
  }

  // Safe access to cmd
  switch (cmd->operation) {
    case 0:
    {
      // Info
      handleInfo(*cmd);
    }
    break;
    case 1:
    {
      // Maxlevel
      handleMaxLevel(*cmd);
    }
    break;
    case 2:
    {
      // Resize
      handleResize(*cmd);
    }
    break;
    case 3:
    {
      // Cutfreq
      handleCutfreq(*cmd);
    }
    break;
    default:
    {
      std::cerr << "Error: Invalid option: " << arguments[3] << '\n';
      return -1;
    }
  }
  return 0;
}
