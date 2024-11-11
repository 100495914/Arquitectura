//
// Created by diego on 20/10/24.
//

#ifndef IMAGESOA_HPP
#define IMAGESOA_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>

constexpr uint8_t MAX_8BIT_VALUE   = 255;
constexpr uint16_t MAX_16BIT_VALUE = 65535;

struct PPMMetadata {
    std::string magicNumber;
    uint width;
    uint height;
    uint16_t maxColorValue;
};

class ImageSOA_8bit;
class ImageSOA_16bit;

class ImageSOA {
  public:
    virtual ~ImageSOA() = default;

    // Delete copy and move operations
    ImageSOA(ImageSOA const &) = delete;

    explicit ImageSOA(PPMMetadata metadata)
      : magicNumber(std::move(metadata.magicNumber)), width(metadata.width),
        height(metadata.height), maxColorValue(metadata.maxColorValue) { }

    ImageSOA & operator=(ImageSOA const &) = delete;

    ImageSOA(ImageSOA &&)             = delete;
    ImageSOA & operator=(ImageSOA &&) = delete;

    virtual void loadData(std::ifstream & file)           = 0;
    virtual void saveToFile(std::string const & filename) = 0;

    // Virtual functions to be implemented by derived classes
    virtual void scaleIntensity(uint newMax) = 0;

    // Setters
    void sWidth(unsigned int const newWidth) { width = newWidth; }

    void sHeight(unsigned int const newHeight) { height = newHeight; }

    void sMaxColorValue(uint16_t const newMaxColorValue) { maxColorValue = newMaxColorValue; }

    void sMagicNumber(std::string const & newMagicNumber) { magicNumber = newMagicNumber; }

  protected:
    [[nodiscard]] uint gWidth() const { return width; }

    [[nodiscard]] uint gHeight() const { return height; }

    [[nodiscard]] uint16_t gMaxColorValue() const { return maxColorValue; }

    [[nodiscard]] std::string const & gMagicNumber() const { return magicNumber; }

  private:
    std::string magicNumber;
    unsigned int width;
    unsigned int height;
    uint16_t maxColorValue;
};

class ImageSOA_8bit : public ImageSOA {
  public:
    explicit ImageSOA_8bit(PPMMetadata const & metadata)
      : ImageSOA(metadata), red(static_cast<size_t>(gWidth() * gHeight())),
        green(static_cast<size_t>(gWidth() * gHeight())),
        blue(static_cast<size_t>(gWidth() * gHeight())) { }

    void loadData(std::ifstream & file) override;
    void saveToFile(std::string const & filename) override;

    [[nodiscard]] std::vector<uint8_t> & gRed() { return red; }

    [[nodiscard]] std::vector<uint8_t> & gGreen() { return green; }

    [[nodiscard]] std::vector<uint8_t> & gBlue() { return blue; }

    void scaleIntensity(uint newMax) override;
    [[nodiscard]] std::unique_ptr<ImageSOA_16bit>
        scaleIntensityChannelSizeChange(uint newMax);

  private:
    std::vector<uint8_t> red;
    std::vector<uint8_t> green;
    std::vector<uint8_t> blue;
};

class ImageSOA_16bit : public ImageSOA {
  public:
    explicit ImageSOA_16bit(PPMMetadata const & metadata)
      : ImageSOA(metadata), red(static_cast<size_t>(gWidth() * gHeight())),
        green(static_cast<size_t>(gWidth() * gHeight())),
        blue(static_cast<size_t>(gWidth() * gHeight())) { }

    void loadData(std::ifstream & file) override;
    void saveToFile(std::string const & filename) override;

    [[nodiscard]] std::vector<uint16_t> & gRed() { return red; }

    [[nodiscard]] std::vector<uint16_t> & gGreen() { return green; }

    [[nodiscard]] std::vector<uint16_t> & gBlue() { return blue; }

    void scaleIntensity(uint newMax) override;
    [[nodiscard]] std::unique_ptr<ImageSOA_8bit> scaleIntensityChannelSizeChange(uint newMax);

  private:
    std::vector<uint16_t> red;
    std::vector<uint16_t> green;
    std::vector<uint16_t> blue;
};

using genericImageSOA =
    std::variant<std::unique_ptr<ImageSOA_16bit>, std::unique_ptr<ImageSOA_8bit>>;

genericImageSOA loadImage(std::string const & filepath);

#endif  // IMAGESOA_HPP
