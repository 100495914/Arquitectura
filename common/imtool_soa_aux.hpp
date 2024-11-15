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
    std::string op1;
    std::string op2;
};

auto sanitizeArgs(std::vector<std::string> const & args) -> std::optional<Command>;
int checkProperArgumentNumber(int operation, size_t argv_size, const Command& cmd);
void handleMaxLevel(Command const & cmd);
void handleResize(Command const & cmd);
int handleCutfreq(Command const & cmd);
void handleInfo(Command const & cmd);

int operate(std::vector<std::string> const& arguments, std::optional<Command> const& cmd);

#endif  // IMTOOL_SOA_AUX_H
