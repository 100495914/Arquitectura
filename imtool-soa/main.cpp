#include "common/imtool_soa_aux.hpp"
#include "common/progargs.hpp"

#include <iostream>
#include <optional>
#include <stdexcept>
#include <variant>

int main(int const argc, char * argv[]) {
  if (argc < 4) {
    printUsage();
    return 1;
  }

  std::vector<std::string> const arguments(argv, argv + argc);

  auto const cmd = sanitizeArgs(arguments);
  if (!cmd) { return -1; }
  if (checkProperArgumentNumber(cmd->operation, arguments.size()) == 0) { return -1; }

  try {
    switch (cmd->operation) {
      case 1:
        {
          // Maxlevel
          handleMaxLevel(cmd.value());
        }
        break;  // Add the break to avoid fall-through to the default case
      case 2:
        {
          // Resize
          handleResize(cmd.value());
        }
      break;
      default:
        {
          std::cerr << "Error: Operacion no reconocida.\n";
          return -1;
        }
    }
  } catch (std::exception const & e) {
    std::cerr << "Error: " << e.what() << "\n";
    return -1;
  }
}
