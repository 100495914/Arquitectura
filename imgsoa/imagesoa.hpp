//
// Created by diego on 20/10/24.
//

#ifndef IMAGESOA_HPP
#define IMAGESOA_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

constexpr uint8_t MAX_8BIT_VALUE   = 255;
constexpr uint16_t MAX_16BIT_VALUE = 65535;

struct PPMMetadata {
    std::string magicNumber;
    uint width;
    uint height;
    uint maxColorValue;
};

struct Dimensions {
    size_t width;
    size_t height;
};

struct PixelPair8 {
    uint8_t lower;
    uint8_t upper;
};

struct PixelPair16 {
    uint16_t lower;
    uint16_t upper;
};

struct Weights {
    float xWeight;
    float yWeight;
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

    virtual void loadData(std::string const & filepath)   = 0;
    virtual void saveToFile(std::string const & filename) = 0;

    // Virtual functions to be implemented by derived classes
    virtual void maxLevel(uint newMax) = 0;

    // Setters
    void sWidth(unsigned int const newWidth) { width = newWidth; }

    void sHeight(unsigned int const newHeight) { height = newHeight; }

    void sMaxColorValue(uint const newMaxColorValue) { maxColorValue = newMaxColorValue; }

    void sMagicNumber(std::string const & newMagicNumber) { magicNumber = newMagicNumber; }

    [[nodiscard]] uint gWidth() const { return width; }

    [[nodiscard]] uint gHeight() const { return height; }

    [[nodiscard]] uint gMaxColorValue() const { return maxColorValue; }

    [[nodiscard]] std::string const & gMagicNumber() const { return magicNumber; }

  private:
    std::string magicNumber;
    unsigned int width;
    unsigned int height;
    uint maxColorValue;
};

class ImageSOA_8bit : public ImageSOA {
  public:
    explicit ImageSOA_8bit(PPMMetadata const & metadata)
      : ImageSOA(metadata), red(static_cast<size_t>(gWidth() * gHeight())),
        green(static_cast<size_t>(gWidth() * gHeight())),
        blue(static_cast<size_t>(gWidth() * gHeight())) { }

    void loadData(std::string const & filepath) override;
    void saveToFile(std::string const & filename) override;

    [[nodiscard]] std::vector<uint8_t> & gRed() { return red; }

    [[nodiscard]] std::vector<uint8_t> & gGreen() { return green; }

    [[nodiscard]] std::vector<uint8_t> & gBlue() { return blue; }

    void maxLevel(uint newMax) override;
    [[nodiscard]] std::unique_ptr<ImageSOA_16bit> maxLevelChangeChannelSize(uint newMax);
    static uint8_t interpolate(PixelPair8 const & lowerPair, PixelPair8 const & upperPair,
                        Weights const & weights);
    std::vector<uint8_t> resizeChannel(Dimensions const & resizeData,
                                       std::vector<uint8_t> const & originalChannel);
    void resizeImage(Dimensions const & resizeData);

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

    void loadData(std::string const & filepath) override;
    void saveToFileBE(std::string const & filename);
    void saveToFile(std::string const & filename) override;

    [[nodiscard]] std::vector<uint16_t> & gRed() { return red; }

    [[nodiscard]] std::vector<uint16_t> & gGreen() { return green; }

    [[nodiscard]] std::vector<uint16_t> & gBlue() { return blue; }

    void maxLevel(uint newMax) override;
    [[nodiscard]] std::unique_ptr<ImageSOA_8bit> maxLevelChangeChannelSize(uint newMax);
    static uint16_t interpolate(PixelPair16 const & lowerPair, PixelPair16 const & upperPair,
                         Weights const & weights);
    std::vector<uint16_t> resizeChannel(Dimensions const & resizeData,
                                        std::vector<uint16_t> const & originalChannel);
    void resizeImage(Dimensions const & resizeData);

  private:
    std::vector<uint16_t> red;
    std::vector<uint16_t> green;
    std::vector<uint16_t> blue;
};

PPMMetadata loadMetadata(std::string const & filepath);

int numberInXbitRange(uint number);

#endif  // IMAGESOA_HPP
