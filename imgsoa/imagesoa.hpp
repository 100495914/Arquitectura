//
// Created by diego on 20/10/24.
//

#ifndef IMAGESOA_HPP
#define IMAGESOA_HPP

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

struct PPMMetadata {
    std::string magicNumber;
    uint width;
    uint height;
    uint16_t maxColorValue;
};

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

    // Virtual functions to be implemented by derived classes
    virtual void resize(uint width, uint height)              = 0;
    virtual void scaleIntensity(uint currentMax, uint newMax) = 0;
    virtual void removeLeastFrequentColors(int n)             = 0;

  protected:
    std::string magicNumber;
    unsigned long width;
    unsigned long height;
    uint16_t maxColorValue;
};

class ImageSOA_8bit final : public ImageSOA {
  public:
    explicit ImageSOA_8bit(PPMMetadata const & metadata)
      : ImageSOA(metadata), red(width * height), green(width * height), blue(width * height) { }

    // Implement the virtual functions for 8-bit images
    void resize(uint width, uint height) override;
    void scaleIntensity(uint currentMax, uint newMax) override;
    void removeLeastFrequentColors(int n) override;
    void compress();

  protected:
    std::vector<uint8_t> red;
    std::vector<uint8_t> green;
    std::vector<uint8_t> blue;
};

class ImageSOA_16bit final : public ImageSOA {
  public:
    explicit ImageSOA_16bit(PPMMetadata const & metadata)
      : ImageSOA(metadata), red(width * height), green(width * height), blue(width * height) { }

    // Implement the virtual functions for 16-bit images
    void resize(uint width, uint height) override;
    void scaleIntensity(uint currentMax, uint newMax) override;
    void removeLeastFrequentColors(int n) override;
    void compress();

  protected:
    std::vector<uint16_t> red;
    std::vector<uint16_t> green;
    std::vector<uint16_t> blue;
};

constexpr int MAX_8BIT_VALUE  = 255;
constexpr int MAX_16BIT_VALUE = 65535;

struct imageDimensions {
    uint width;
    uint height;
};

#endif  // IMAGESOA_HPP
