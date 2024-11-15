#include <gtest/gtest.h>
#include "../imgaos/imageaos.hpp"
#include <vector>
#include <fstream>
#include <cmath>

static constexpr uint16_t doscientos = 200;
static constexpr uint16_t doscincocincuenta= 250;
static constexpr uint16_t cincuenta = 50;
static constexpr uint16_t cientocincuenta = 150;
static constexpr uint16_t cien = 100;
static constexpr uint16_t doscincocinco = 255;
static constexpr double interpolationFactor = 0.5;
static constexpr double epsilon = 1e-5;

namespace {
  // Función auxiliar para crear un Pixel y facilitar las verificaciones
  Pixel createPixel(uint16_t red, uint16_t green, uint16_t blue) {
    return Pixel{.red = red, .green = green, .blue = blue};
  }
}

// Prueba para verificar que las intensidades de los píxeles se escalan correctamente
TEST(ScaleIntensityTest, ScalesPixelIntensities) {
    std::vector<Pixel> pixels = {createPixel(cien, cientocincuenta, doscientos), createPixel(cincuenta, cien, cientocincuenta)};
    scaleIntensity(pixels, doscientos, cien);

    EXPECT_EQ(pixels[0], createPixel(50, 75, cien));
    EXPECT_EQ(pixels[1], createPixel(25, 50, 75));
}

// Prueba para verificar la interpolación entre dos valores de canal de color
TEST(InterpolateChannelTest, InterpolatesBetweenTwoChannels) {
    EXPECT_NEAR(interpolateChannel(100.0, 200.0, interpolationFactor), 150.0, epsilon);
    EXPECT_NEAR(interpolateChannel(0.0, 100.0, 0.25), 25.0, epsilon);
}

// Prueba para verificar la interpolación entre dos píxeles
TEST(InterpolatePixelTest, InterpolatesBetweenTwoPixels) {
    const Pixel pixel1 = createPixel(cien, cien, cien);
    const Pixel pixel2 = createPixel(doscientos, doscientos, doscientos);

    const Pixel result = interpolate(pixel1, pixel2, interpolationFactor);
    EXPECT_EQ(result, createPixel(cientocincuenta, cientocincuenta, cientocincuenta));
}

// Prueba para verificar el cambio de tamaño de una imagen con interpolación
TEST(ResizeImageTest, ResizesImageWithInterpolation) {
    const PPMMetadata metadata = {.magicNumber = "P6", .width = 2, .height = 2, .maxColorValue = doscincocinco};
    const std::vector<Pixel> pixels = {createPixel(cien, cien, cien), createPixel(doscientos, doscientos, doscientos),
                                 createPixel(cientocincuenta, cientocincuenta, cientocincuenta), createPixel(doscincocincuenta, doscincocincuenta, doscincocincuenta)};

    const auto resizedPixels = resizeImage(pixels, metadata, 1, 1);

    EXPECT_EQ(resizedPixels.size(), 1);
    EXPECT_EQ(resizedPixels[0], createPixel(175, 175, 175));
}

// Prueba para verificar el guardado y la carga de una imagen en formato PPM
TEST(SaveAndLoadImageTest, SaveAndLoadPPMImage) {
    const std::string filename = "test_image.ppm";
    const PPMMetadata metadata = {.magicNumber = "P6", .width = 2, .height = 2, .maxColorValue = doscincocinco};
    const std::vector<Pixel> pixels = {createPixel(cien, cien, cien), createPixel(doscientos, doscientos, doscientos),
                                 createPixel(cientocincuenta, cientocincuenta, cientocincuenta), createPixel(doscincocincuenta, doscincocincuenta, doscincocincuenta)};

    saveImage(filename, pixels, metadata);

    const std::vector<Pixel> loadedPixels = loadImage(filename, metadata);
    EXPECT_EQ(pixels, loadedPixels);

    static_cast<void>(std::remove(filename.c_str()));  // Ignora explícitamente el valor de retorno
}

// Prueba para verificar la lectura de los metadatos de un archivo PPM
TEST(GetPPMMetadataTest, ReadsPPMMetadata) {
    const std::string filename = "test_metadata.ppm";
    std::ofstream file(filename);
    file << "P6\n2 2\n" << doscincocinco << "\n";
    file.close();

    const PPMMetadata metadata = getPPMMetadata(filename);
    EXPECT_EQ(metadata.magicNumber, "P6");
    EXPECT_EQ(metadata.width, 2);
    EXPECT_EQ(metadata.height, 2);
    EXPECT_EQ(metadata.maxColorValue, doscincocinco);

    static_cast<void>(std::remove(filename.c_str()));  // Ignora explícitamente el valor de retorno
}

// Prueba para verificar el cálculo de la distancia entre dos colores
TEST(ColorDistanceTest, CalculatesDistanceBetweenColors) {
    const Pixel pixel1 = createPixel(cien, cien, cien);
    const Pixel pixel2 = createPixel(doscientos, doscientos, doscientos);
    const int distance = colorDistance(pixel1, pixel2);
    EXPECT_EQ(distance, 30000);
}

// Prueba para verificar la sustitución de los colores menos frecuentes en un conjunto de píxeles
TEST(RemoveLeastFrequentColorsTest, ReplacesLeastFrequentColors) {
    const std::vector<Pixel> pixels = {createPixel(cien, cien, cien), createPixel(doscientos, doscientos, doscientos),
                                 createPixel(cien, cien, cien), createPixel(250, 250, 250)};

    const auto modifiedPixels = removeLeastFrequentColors(pixels, 1);
    EXPECT_EQ(modifiedPixels[1], modifiedPixels[0]); // El píxel (200, 200, 200) debe ser reemplazado
}

// Función  para ejecutar todos los tests
GTEST_API_ int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
