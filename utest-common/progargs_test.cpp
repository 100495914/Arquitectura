//
// Created by diego on 20/10/24.
//

#include "../common/progargs.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <string>

// Test para la operación "info" con argumentos correctos
TEST(ProcessArgsTest, InfoOperationValidArgs) {
    const std::vector<std::string> arguments = {"program", "input.ppm", "output.ppm", "info"};
    const ProgramArgs result = processArgs(arguments);
    EXPECT_EQ(result.inputFile, "input.ppm");
    EXPECT_EQ(result.outputFile, "output.ppm");
    EXPECT_EQ(result.operation, "info");
    EXPECT_TRUE(result.extraParams.empty());
}

// Test para operación "maxlevel" con argumentos correctos
TEST(ProcessArgsTest, MaxLevelOperationValidArgs) {
    const std::vector<std::string> arguments = {"program", "input.ppm", "output.ppm", "maxlevel", "128"};
    ProgramArgs result = processArgs(arguments);
    EXPECT_EQ(result.inputFile, "input.ppm");
    EXPECT_EQ(result.outputFile, "output.ppm");
    EXPECT_EQ(result.operation, "maxlevel");
    ASSERT_EQ(result.extraParams.size(), 1);
    EXPECT_EQ(result.extraParams[0], "128");
}

// Test para operación "resize" con argumentos correctos
TEST(ProcessArgsTest, ResizeOperationValidArgs) {
    const std::vector<std::string> arguments = {"program", "input.ppm", "output.ppm", "resize", "200", "300"};
    ProgramArgs result = processArgs(arguments);
    EXPECT_EQ(result.inputFile, "input.ppm");
    EXPECT_EQ(result.outputFile, "output.ppm");
    EXPECT_EQ(result.operation, "resize");
    ASSERT_EQ(result.extraParams.size(), 2);
    EXPECT_EQ(result.extraParams[0], "200");
    EXPECT_EQ(result.extraParams[1], "300");
}

// Test para operación "cutfreq" con argumentos correctos
TEST(ProcessArgsTest, CutFreqOperationValidArgs) {
    const std::vector<std::string> arguments = {"program", "input.ppm", "output.ppm", "cutfreq", "10"};
    ProgramArgs result = processArgs(arguments);
    EXPECT_EQ(result.inputFile, "input.ppm");
    EXPECT_EQ(result.outputFile, "output.ppm");
    EXPECT_EQ(result.operation, "cutfreq");
    ASSERT_EQ(result.extraParams.size(), 1);
    EXPECT_EQ(result.extraParams[0], "10");
}

// Test para operación "compress" con argumentos correctos
TEST(ProcessArgsTest, CompressOperationValidArgs) {
    const std::vector<std::string> arguments = {"program", "input.ppm", "output.ppm", "compress"};
    const ProgramArgs result = processArgs(arguments);
    EXPECT_EQ(result.inputFile, "input.ppm");
    EXPECT_EQ(result.outputFile, "output.ppm");
    EXPECT_EQ(result.operation, "compress");
    EXPECT_TRUE(result.extraParams.empty());
}

// Test para cantidad insuficiente de argumentos
TEST(ProcessArgsTest, InvalidNumberOfArgs) {
    const std::vector<std::string> arguments = {"program", "input.ppm", "output.ppm"};
    EXPECT_THROW(processArgs(arguments), std::runtime_error);
}

// Test para operación no válida
TEST(ProcessArgsTest, InvalidOperation) {
    const std::vector<std::string> arguments = {"program", "input.ppm", "output.ppm", "invalidop"};
    EXPECT_THROW(processArgs(arguments), std::runtime_error);
}

// Test para argumentos adicionales incorrectos en "info"
TEST(ProcessArgsTest, ExtraArgsInfoOperation) {
    const std::vector<std::string> arguments = {"program", "input.ppm", "output.ppm", "info", "extra"};
    EXPECT_THROW(processArgs(arguments), std::runtime_error);
}

// Test para valor de "maxlevel" fuera de rango
TEST(ValidateMaxLevelTest, MaxLevelOutOfRange) {
    const std::vector<std::string> arguments = {"program", "input.ppm", "output.ppm", "maxlevel", "70000"};
    EXPECT_THROW(processArgs(arguments), std::runtime_error);
}

// Test para valores no numéricos en "maxlevel"
TEST(ValidateMaxLevelTest, MaxLevelNonNumeric) {
    const std::vector<std::string> arguments = {"program", "input.ppm", "output.ppm", "maxlevel", "abc"};
    EXPECT_THROW(processArgs(arguments), std::runtime_error);
}

// Test para un valor de ancho negativo en el comando "resize"
TEST(ValidateResizeTest, ResizeInvalidWidth) {
  const std::vector<std::string> arguments = {"program", "input.ppm", "output.ppm", "resize", "-200", "300"};
  EXPECT_THROW(processArgs(arguments), std::runtime_error);
}

// Test para un valor de alto negativo en el comando "resize"
TEST(ValidateResizeTest, ResizeInvalidHeight) {
  const std::vector<std::string> arguments = {"program", "input.ppm", "output.ppm", "resize", "200", "-300"};
  EXPECT_THROW(processArgs(arguments), std::runtime_error);
}

// Test para valor de "cutfreq" no numérico
TEST(ValidateCutFreqTest, CutFreqNonNumeric) {
    const std::vector<std::string> arguments = {"program", "input.ppm", "output.ppm", "cutfreq", "abc"};
    EXPECT_THROW(processArgs(arguments), std::runtime_error);
}

// Test para valor de "cutfreq" inválido
TEST(ValidateCutFreqTest, CutFreqInvalid) {
    const std::vector<std::string> arguments = {"program", "input.ppm", "output.ppm", "cutfreq", "-10"};
    EXPECT_THROW(processArgs(arguments), std::runtime_error);
}