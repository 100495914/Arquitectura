#include "common/imtool_soa_aux.hpp"
#include "common/progargs.hpp"
#include "imgsoa/imagesoa.hpp"


#include <iostream>
#include <optional>

int main(int const argc, char * argv[]) {
  std::vector<std::string> const arguments(argv, argv + argc);
  if (argc < 3) {
    std::cout << "Error: Invalid number of arguments: " << arguments.size() << '\n';
    return 1;
  }
  auto const cmd = sanitizeArgs(arguments);
  if (!cmd) { return -1; }
  if (cmd->operation == -1) { return -1; }
  if (checkProperArgumentNumber(cmd->operation, arguments.size(), cmd.value()) == 0) { return -1; }
  try {
    return operate(arguments, cmd);
  } catch (std::exception const & e) {
    std::cerr << "Error: " << e.what() << "\n";
    return -1;
  }
}
