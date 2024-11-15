// Created by diego on 15/11/24.

#include <gtest/gtest.h>
#include "../common/PPMMetadata.hpp"
#include "getPPMMetadata_test.hpp"
#include <sstream>
#include <stdexcept>

// Función auxiliar para obtener metadata desde un flujo de entrada (en lugar de un archivo real)
PPMMetadata getPPMMetadataFromStream(std::istream& file) {
  PPMMetadata metadata;

  // Leer el número mágico
  file >> metadata.magicNumber;
  if (metadata.magicNumber != "P6") {
    throw std::runtime_error("Error: Formato PPM inválido.");
  }

  // Leer la anchura, altura y valor máximo de color
  file >> metadata.width >> metadata.height >> metadata.maxColorValue;

  // Validar las dimensiones
  if (metadata.width <= 0 || metadata.height <= 0) {
    throw std::runtime_error("Error: Dimensiones inválidas.");
  }

  // Validar el valor máximo de color
  constexpr int MaxColorValue = 65535;
  if (metadata.maxColorValue <= 0 || metadata.maxColorValue > MaxColorValue) {
    throw std::runtime_error("Error: Valor máximo de color fuera de rango.");
  }

  // Ignorar el carácter de nueva línea después de los metadatos
  file.ignore(1);

  return metadata;
}


// Prueba para un archivo PPM válido
TEST(GetPPMMetadataTest, ValidPPMFile) {
    std::stringstream mockFile("P6\n100 200\n255\n");
    const PPMMetadata metadata = getPPMMetadataFromStream(mockFile);

    EXPECT_EQ(metadata.magicNumber, "P6");
    EXPECT_EQ(metadata.width, 100);
    EXPECT_EQ(metadata.height, 200);
    EXPECT_EQ(metadata.maxColorValue, 255);
}

// Prueba para un archivo PPM con un número mágico inválido
TEST(GetPPMMetadataTest, InvalidMagicNumber) {
    std::stringstream mockFile("P3\n100 200\n255\n");
    EXPECT_THROW({
        getPPMMetadataFromStream(mockFile);
    }, std::runtime_error);
}

// Prueba para un archivo PPM con un valor máximo de color inválido (fuera de rango)
TEST(GetPPMMetadataTest, InvalidMaxColorValue) {
    std::stringstream mockFile("P6\n100 200\n70000\n");
    EXPECT_THROW({
        getPPMMetadataFromStream(mockFile);
    }, std::runtime_error);
}

// Prueba para un archivo PPM con un valor máximo de color negativo
TEST(GetPPMMetadataTest, NegativeMaxColorValue) {
    std::stringstream mockFile("P6\n100 200\n-1\n");
    EXPECT_THROW({
        getPPMMetadataFromStream(mockFile);
    }, std::runtime_error);
}

// Prueba para un archivo PPM con dimensiones inválidas (anchura negativa)
TEST(GetPPMMetadataTest, InvalidDimensions) {
    std::stringstream mockFile("P6\n-100 200\n255\n");
    EXPECT_THROW({
        getPPMMetadataFromStream(mockFile);
    }, std::runtime_error);
}

// Prueba para un archivo PPM con dimensiones inválidas (altura negativa)
TEST(GetPPMMetadataTest, InvalidHeight) {
  std::stringstream mockFile("P6\n100 -200\n255\n");
  EXPECT_THROW({
      getPPMMetadataFromStream(mockFile);
  }, std::runtime_error);
}


// Prueba para un archivo PPM con el valor máximo de color faltante
TEST(GetPPMMetadataTest, MissingMaxColorValue) {
    std::stringstream mockFile("P6\n100 200\n");
    EXPECT_THROW({
        getPPMMetadataFromStream(mockFile);
    }, std::runtime_error);
}

// Prueba para un archivo PPM que carece de un salto de línea después de los metadatos
TEST(GetPPMMetadataTest, MissingNewlineAfterMetadata) {
    std::stringstream mockFile("P6\n100 200\n255");
    const PPMMetadata metadata = getPPMMetadataFromStream(mockFile);

    EXPECT_EQ(metadata.magicNumber, "P6");
    EXPECT_EQ(metadata.width, 100);
    EXPECT_EQ(metadata.height, 200);
    EXPECT_EQ(metadata.maxColorValue, 255);
}
