//
// Created by diego on 20/10/24.
//

#ifndef IMAGESOA_HPP
#define IMAGESOA_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

constexpr uint8_t MAX_8BIT_VALUE = 255;
constexpr uint16_t MAX_16BIT_VALUE = 65535;



struct PPMMetadata {
  std::string magicNumber = "P6";
  size_t width = 0;
  size_t height = 0;
  uint maxColorValue = 0;
};

PPMMetadata loadMetadata(std::string const & filepath);

int numberInXbitRange(uint number);

struct Dimensions {
  size_t width = 0;
  size_t height = 0;
};

struct Point {
  int x_coord = 0;
  int y_coord = 0;
};

struct RGB8 {
  uint8_t r, g, b;

  bool operator==(const RGB8 & other) const {
    return r == other.r && g == other.g && b == other.b;
  }
};

struct RGB16 {
  uint16_t r, g, b;

  bool operator==(const RGB16 & other) const {
    return r == other.r && g == other.g && b == other.b;
  }
};

constexpr uint ocho = 8;
constexpr uint dieciseis = 16;
constexpr uint treintaydos = 32;

// Hash functions for unordered_map
namespace std {
  template <>
  struct hash<RGB8> {
    size_t operator()(const RGB8 & color) const noexcept {
      return (static_cast<size_t>(color.r) << dieciseis) |
             (static_cast<size_t>(color.g) << ocho) | color.b;
    }
  };

  template <>
  struct hash<RGB16> {
    size_t operator()(const RGB16 & color) const noexcept {
      return (static_cast<size_t>(color.r) << treintaydos) |
             (static_cast<size_t>(color.g) << dieciseis) | color.b;
    }
  };
} // namespace std

class ImageSOA_8bit;
class ImageSOA_16bit;

class ImageSOA {
  public:
    virtual ~ImageSOA() = default;

    // Delete copy operations
    ImageSOA(ImageSOA const &) = delete;

    // Define move constructor
    ImageSOA(ImageSOA &&) = default;

    // Define move assignment operator
    ImageSOA & operator=(ImageSOA &&) = default;

    explicit ImageSOA(PPMMetadata metadata)
      : magicNumber(std::move(metadata.magicNumber)), width(metadata.width),
        height(metadata.height), maxColorValue(metadata.maxColorValue) {}

    ImageSOA & operator=(ImageSOA const &) = delete;

    // Setters
    void sWidth(size_t const newWidth) { width = newWidth; }

    void sHeight(size_t const newHeight) { height = newHeight; }

    void sMaxColorValue(uint const newMaxColorValue) { maxColorValue = newMaxColorValue; }

    void sMagicNumber(std::string const & newMagicNumber) { magicNumber = newMagicNumber; }

    [[nodiscard]] size_t gWidth() const { return width; }

    [[nodiscard]] size_t gHeight() const { return height; }

    [[nodiscard]] uint gMaxColorValue() const { return maxColorValue; }

    [[nodiscard]] std::string const & gMagicNumber() const { return magicNumber; }

  private:
    std::string magicNumber;
    size_t width;
    size_t height;
    uint maxColorValue;
};

class ImageSOA_8bit final : public ImageSOA {
  public:
    explicit ImageSOA_8bit(PPMMetadata const & metadata)
      : ImageSOA(metadata), red(gWidth() * gHeight()), green(gWidth() * gHeight()),
        blue(gWidth() * gHeight()) {}

    bool operator==(ImageSOA_8bit const & other) const;

    void loadData(std::string const & filepath);
    void saveToFile(std::string const & filename);

    [[nodiscard]] std::vector<uint8_t> & gRed() { return red; }

    [[nodiscard]] std::vector<uint8_t> & gGreen() { return green; }

    [[nodiscard]] std::vector<uint8_t> & gBlue() { return blue; }

    void maxLevel(uint newMax);
    [[nodiscard]] std::unique_ptr<ImageSOA_16bit> maxLevelChangeChannelSize(uint newMax);
    static int calculatePosition(Point point, Dimensions dim);
    void resize(Dimensions dim);
    static double helper_resizeInterpolate(std::vector<uint8_t> & channel,
                                           Dimensions original_dimensions, double x_target,
                                           double y_target);
    std::vector<uint8_t> resize_helper(std::vector<uint8_t> & channel, Dimensions dim) const;
    void reduceColors(size_t n);

  private:
    std::vector<uint8_t> red;
    std::vector<uint8_t> green;
    std::vector<uint8_t> blue;
    [[nodiscard]] std::unordered_map<RGB8, size_t> computeColorFrequencies() const;
    [[nodiscard]] static std::vector<RGB8>
      findLeastFrequentColors(std::unordered_map<RGB8, size_t> const & freqs, size_t n);
    [[nodiscard]] static RGB8 findNearestColor(const RGB8 & target,
                                               std::unordered_set<RGB8> const & validColors);
    void replaceColors(std::unordered_map<RGB8, RGB8> const & colorMap);
    [[nodiscard]] static double colorDistance(const RGB8 & var_c1, const RGB8 & var_c2);
};

class ImageSOA_16bit final : public ImageSOA {
  public:
    explicit ImageSOA_16bit(PPMMetadata const & metadata)
      : ImageSOA(metadata), red(gWidth() * gHeight()), green(gWidth() * gHeight()),
        blue(gWidth() * gHeight()) {}

    bool operator==(ImageSOA_16bit const & other) const;
    void loadData(std::string const & filepath);
    void saveToFileBE(std::string const & filename);
    void saveToFile(std::string const & filename);

    [[nodiscard]] std::vector<uint16_t> & gRed() { return red; }

    [[nodiscard]] std::vector<uint16_t> & gGreen() { return green; }

    [[nodiscard]] std::vector<uint16_t> & gBlue() { return blue; }

    void maxLevel(uint newMax);
    [[nodiscard]] std::unique_ptr<ImageSOA_8bit> maxLevelChangeChannelSize(uint newMax);
    static int calculatePosition(Point point, Dimensions dim);
    void resize(Dimensions dim);
    static double helper_resizeInterpolate(std::vector<uint16_t> & channel,
                                           Dimensions original_dimensions, double x_target,
                                           double y_target);
    std::vector<uint16_t> resize_helper(std::vector<uint16_t> & channel, Dimensions dim) const;
    void reduceColors(size_t n);

  private:
    std::vector<uint16_t> red;
    std::vector<uint16_t> green;
    std::vector<uint16_t> blue;
    [[nodiscard]] std::unordered_map<RGB16, size_t> computeColorFrequencies() const;
    [[nodiscard]] static std::vector<RGB16>
      findLeastFrequentColors(std::unordered_map<RGB16, size_t> const & freqs, size_t n);
    [[nodiscard]] static RGB16 findNearestColor(const RGB16 & target,
                                                std::unordered_set<RGB16> const & validColors);
    void replaceColors(std::unordered_map<RGB16, RGB16> const & colorMap);
    [[nodiscard]] static double colorDistance(const RGB16 & var_c1, const RGB16 & var_c2);
};

#endif  // IMAGESOA_HPP