//
// Created by Alberto on 13/11/2024.
//

#ifndef IMTOOL_SOA_AUX_H
#define IMTOOL_SOA_AUX_H

#include <optional>
#include <string>
#include <vector>

struct Command {
  std::string input;
  std::string output;
  int operation;
  int op1 = 0;  // Default to 0 in case they are not provided
  int op2 = 0;
};

auto sanitizeArgs(std::vector<std::string> const & args)  -> std::optional<Command>;
int checkProperArgumentNumber(int operation, size_t argv_size);
void handleMaxLevel(Command const & cmd);
void handleResize(Command const & cmd);

#endif //IMTOOL_SOA_AUX_H
