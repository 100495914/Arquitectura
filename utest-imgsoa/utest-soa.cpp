//
// Created by diego on 20/10/24.
//

#include "../imgsoa/imagesoa.hpp"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>

namespace {


  std::string test_wrapper(void (*func)()) {
    auto const start = std::chrono::high_resolution_clock::now();
    func();
    auto const end = std::chrono::high_resolution_clock::now();

    // Calculate the duration in seconds
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    // Convert the duration to a string
    std::ostringstream oss;
    oss << duration.count();
    return oss.str();
  }

  void test_comparator() {
    std::string const input1    = "../../input/lake-large.ppm";
    PPMMetadata const metadata1 = loadMetadata(input1);
    auto const image1           = std::make_unique<ImageSOA_8bit>(metadata1);
    auto const image2           = std::make_unique<ImageSOA_8bit>(metadata1);
    image1->loadData(input1);
    image2->loadData(input1);
    if (!(*image1 == *image2)) {
      std::cerr << "Test comparator failed!"
                << " \n";
    } else {
      std::cout << "Test comparator passed!"
                << " \n";
    }
  }

  [[maybe_unused]] void test_cutFreqLake100k() {
    std::string const input1    = "../../input/lake-large.ppm";
    std::string const input2    = "../../output/cutfreq/lake-large-100K.ppm";
    constexpr int nColors       = 100000;
    PPMMetadata const metadata1 = loadMetadata(input1);
    PPMMetadata const metadata2 = loadMetadata(input2);
    auto const image1           = std::make_unique<ImageSOA_8bit>(metadata1);
    auto const image2           = std::make_unique<ImageSOA_8bit>(metadata1);
    image1->loadData(input1);
    image2->loadData(input2);
    image1->reduceColors(nColors);
    if (!(*image1 == *image2)) {
      std::cerr << "Test cutFreq lake-large-100k failed!"
                << " \n";
    } else {
      std::cout << "Test cutFreq lake-large-100k passed!"
                << " \n";
    }
  }

  [[maybe_unused]] void test_cutFreqLake162k() {
    std::string const input1    = "../../input/lake-large.ppm";
    std::string const input2    = "../../output/cutfreq/lake-large-162K.ppm";
    constexpr int nColors       = 162000;
    PPMMetadata const metadata1 = loadMetadata(input1);
    PPMMetadata const metadata2 = loadMetadata(input2);
    auto const image1           = std::make_unique<ImageSOA_8bit>(metadata1);
    auto const image2           = std::make_unique<ImageSOA_8bit>(metadata1);
    image1->loadData(input1);
    image2->loadData(input2);
    image1->reduceColors(nColors);
    if (!(*image1 == *image2)) {
      std::cerr << "Test cutFreq lake-large-162k failed!" << '\n';
    } else {
      std::cout << "Test cutFreq lake-large-162k passed!" << '\n';
    }
  }

  [[maybe_unused]] void test_resizeDeerLarge100() {
    std::string const input1    = "../../input/deer-large.ppm";
    std::string const input2    = "../../output/resize/deer-large-100.ppm";
    constexpr size_t newWidth   = 100;
    constexpr size_t newHeight  = 100;
    constexpr Dimensions dim    = {.width = newWidth, .height = newHeight};
    PPMMetadata const metadata1 = loadMetadata(input1);
    PPMMetadata const metadata2 = loadMetadata(input2);
    auto const image1           = std::make_unique<ImageSOA_8bit>(metadata1);
    auto const image2           = std::make_unique<ImageSOA_8bit>(metadata2);
    image1->loadData(input1);
    image2->loadData(input2);
    image1->resize(dim);
    if (!(*image1 == *image2)) {
      std::cerr << "Test resize deer-large-100 failed!" << '\n';
    } else {
      std::cout << "Test resize deer-large-100 passed!" << '\n';
    }
  }

  [[maybe_unused]] void test_resizeDeerLarge1000() {
    std::string const input1    = "../../input/deer-large.ppm";
    std::string const input2    = "../../output/resize/deer-large-1000.ppm";
    constexpr size_t newWidth   = 1000;
    constexpr size_t newHeight  = 1000;
    constexpr Dimensions dim    = {.width = newWidth, .height = newHeight};
    PPMMetadata const metadata1 = loadMetadata(input1);
    PPMMetadata const metadata2 = loadMetadata(input2);
    auto const image1           = std::make_unique<ImageSOA_8bit>(metadata1);
    auto const image2           = std::make_unique<ImageSOA_8bit>(metadata2);
    image1->loadData(input1);
    image2->loadData(input2);
    image1->resize(dim);
    if (!(*image1 == *image2)) {
      std::cerr << "Test resize deer-large-1000 failed!" << '\n';
    } else {
      std::cout << "Test resize deer-large-1000 passed!" << '\n';
    }
  }

  [[maybe_unused]] void test_resizeDeerSmall100() {
    std::string const input1    = "../../input/deer-small.ppm";
    std::string const input2    = "../../output/resize/deer-small-100.ppm";
    constexpr size_t newWidth   = 100;
    constexpr size_t newHeight  = 100;
    constexpr Dimensions dim    = {.width = newWidth, .height = newHeight};
    PPMMetadata const metadata1 = loadMetadata(input1);
    PPMMetadata const metadata2 = loadMetadata(input2);
    auto const image1           = std::make_unique<ImageSOA_8bit>(metadata1);
    auto const image2           = std::make_unique<ImageSOA_8bit>(metadata2);
    image1->loadData(input1);
    image2->loadData(input2);
    image1->resize(dim);
    if (!(*image1 == *image2)) {
      std::cerr << "Test resize deer-small-100 failed!" << '\n';
    } else {
      std::cout << "Test resize deer-small-100 passed!" << '\n';
    }
  }

  [[maybe_unused]] void test_resizeDeerSmall1000() {
    std::string const input1    = "../../input/deer-small.ppm";
    std::string const input2    = "../../output/resize/deer-small-1000.ppm";
    constexpr size_t newWidth   = 1000;
    constexpr size_t newHeight  = 1000;
    constexpr Dimensions dim    = {.width = newWidth, .height = newHeight};
    PPMMetadata const metadata1 = loadMetadata(input1);
    PPMMetadata const metadata2 = loadMetadata(input2);
    auto const image1           = std::make_unique<ImageSOA_8bit>(metadata1);
    auto const image2           = std::make_unique<ImageSOA_8bit>(metadata2);
    image1->loadData(input1);
    image2->loadData(input2);
    image1->resize(dim);
    if (!(*image1 == *image2)) {
      std::cerr << "Test resize deer-small-1000 failed!" << '\n';
    } else {
      std::cout << "Test resize deer-small-1000 passed!" << '\n';
    }
  }

  [[maybe_unused]] void test_resizeLakeLarge100() {
    std::string const input1    = "../../input/lake-large.ppm";
    std::string const input2    = "../../output/resize/lake-large-100.ppm";
    constexpr size_t newWidth   = 100;
    constexpr size_t newHeight  = 100;
    constexpr Dimensions dim    = {.width = newWidth, .height = newHeight};
    PPMMetadata const metadata1 = loadMetadata(input1);
    PPMMetadata const metadata2 = loadMetadata(input2);
    auto const image1           = std::make_unique<ImageSOA_8bit>(metadata1);
    auto const image2           = std::make_unique<ImageSOA_8bit>(metadata2);
    image1->loadData(input1);
    image2->loadData(input2);
    image1->resize(dim);
    if (!(*image1 == *image2)) {
      std::cerr << "Test resize lake-large-100 failed!" << '\n';
    } else {
      std::cout << "Test resize lake-large-100 passed!" << '\n';
    }
  }

  [[maybe_unused]] void test_resizeLakeLarge1000() {
    std::string const input1    = "../../input/lake-large.ppm";
    std::string const input2    = "../../output/resize/lake-large-1000.ppm";
    constexpr size_t newWidth   = 1000;
    constexpr size_t newHeight  = 1000;
    constexpr Dimensions dim    = {.width = newWidth, .height = newHeight};
    PPMMetadata const metadata1 = loadMetadata(input1);
    PPMMetadata const metadata2 = loadMetadata(input2);
    auto const image1           = std::make_unique<ImageSOA_8bit>(metadata1);
    auto const image2           = std::make_unique<ImageSOA_8bit>(metadata2);
    image1->loadData(input1);
    image2->loadData(input2);
    image1->resize(dim);
    if (!(*image1 == *image2)) {
      std::cerr << "Test resize lake-large-1000 failed!" << '\n';
    } else {
      std::cout << "Test resize lake-large-1000 passed!" << '\n';
    }
  }

  [[maybe_unused]] void test_resizeLakeSmall100() {
    std::string const input1    = "../../input/lake-small.ppm";
    std::string const input2    = "../../output/resize/lake-small-100.ppm";
    constexpr size_t newWidth   = 100;
    constexpr size_t newHeight  = 100;
    constexpr Dimensions dim    = {.width = newWidth, .height = newHeight};
    PPMMetadata const metadata1 = loadMetadata(input1);
    PPMMetadata const metadata2 = loadMetadata(input2);
    auto const image1           = std::make_unique<ImageSOA_8bit>(metadata1);
    auto const image2           = std::make_unique<ImageSOA_8bit>(metadata2);
    image1->loadData(input1);
    image2->loadData(input2);
    image1->resize(dim);
    if (!(*image1 == *image2)) {
      std::cerr << "Test resize lake-small-100 failed!" << '\n';
    } else {
      std::cout << "Test resize lake-small-100 passed!" << '\n';
    }
  }

  [[maybe_unused]] void test_resizeLakeSmall1000() {
    std::string const input1    = "../../input/lake-small.ppm";
    std::string const input2    = "../../output/resize/lake-small-1000.ppm";
    constexpr size_t newWidth   = 1000;
    constexpr size_t newHeight  = 1000;
    constexpr Dimensions dim    = {.width = newWidth, .height = newHeight};
    PPMMetadata const metadata1 = loadMetadata(input1);
    PPMMetadata const metadata2 = loadMetadata(input2);
    auto const image1           = std::make_unique<ImageSOA_8bit>(metadata1);
    auto const image2           = std::make_unique<ImageSOA_8bit>(metadata2);
    image1->loadData(input1);
    image2->loadData(input2);
    image1->resize(dim);
    if (!(*image1 == *image2)) {
      std::cerr << "Test resize lake-small-1000 failed!" << '\n';
    } else {
      std::cout << "Test resize lake-small-1000 passed!" << '\n';
    }
  }

  [[maybe_unused]] void test_maxlevelDeerSmall100() {
    std::string const input1     = "../../input/deer-small.ppm";
    std::string const input2     = "../../output/maxlevel/deer-small-100.ppm";
    constexpr size_t newMaxColor = 100;
    PPMMetadata const metadata1  = loadMetadata(input1);
    PPMMetadata const metadata2  = loadMetadata(input2);
    auto const image1            = std::make_unique<ImageSOA_8bit>(metadata1);
    auto const image2            = std::make_unique<ImageSOA_8bit>(metadata2);
    image1->loadData(input1);
    image2->loadData(input2);
    image1->maxLevel(newMaxColor);
    if (!(*image1 == *image2)) {
      std::cerr << "Test maxlevel deer-small-100 failed!" << '\n';
    } else {
      std::cout << "Test maxlevel deer-small-100 passed!" << '\n';
    }
  }

  [[maybe_unused]] void test_maxlevelDeerSmall255() {
    std::string const input1     = "../../input/deer-small.ppm";
    std::string const input2     = "../../output/maxlevel/deer-small-255.ppm";
    constexpr size_t newMaxColor = 255;
    PPMMetadata const metadata1  = loadMetadata(input1);
    PPMMetadata const metadata2  = loadMetadata(input2);
    auto const image1            = std::make_unique<ImageSOA_8bit>(metadata1);
    auto const image2            = std::make_unique<ImageSOA_8bit>(metadata2);
    image1->loadData(input1);
    image2->loadData(input2);
    image1->maxLevel(newMaxColor);
    if (!(*image1 == *image2)) {
      std::cerr << "Test maxlevel deer-small-255 failed!" << '\n';
    } else {
      std::cout << "Test maxlevel deer-small-255 passed!" << '\n';
    }
  }

  [[maybe_unused]] void test_maxlevelDeerSmall1000() {
    std::string const input1     = "../../input/deer-small.ppm";
    std::string const input2     = "../../output/maxlevel/deer-small-1000.ppm";
    constexpr size_t newMaxColor = 1000;
    PPMMetadata const metadata1  = loadMetadata(input1);
    PPMMetadata const metadata2  = loadMetadata(input2);
    auto const image1            = std::make_unique<ImageSOA_8bit>(metadata1);
    auto const image2            = std::make_unique<ImageSOA_16bit>(metadata2);
    image1->loadData(input1);
    image2->loadData(input2);
    std::unique_ptr<ImageSOA_16bit> const image3 = image1->maxLevelChangeChannelSize(newMaxColor);
    if (!(*image3 == *image2)) {
      std::cerr << "Test maxlevel deer-small-1000 failed!" << '\n';
    } else {
      std::cout << "Test maxlevel deer-small-1000 passed!" << '\n';
    }
  }

  [[maybe_unused]] void test_maxlevelDeerSmall65535() {
    std::string const input1     = "../../input/deer-small.ppm";
    std::string const input2     = "../../output/maxlevel/deer-small-65535.ppm";
    constexpr size_t newMaxColor = 65535;
    PPMMetadata const metadata1  = loadMetadata(input1);
    PPMMetadata const metadata2  = loadMetadata(input2);
    auto const image1            = std::make_unique<ImageSOA_8bit>(metadata1);
    auto const image2            = std::make_unique<ImageSOA_16bit>(metadata2);
    image1->loadData(input1);
    image2->loadData(input2);
    std::unique_ptr<ImageSOA_16bit> const image3 = image1->maxLevelChangeChannelSize(newMaxColor);
    if (!(*image3 == *image2)) {
      std::cerr << "Test maxlevel lake-small-65535 failed!" << '\n';
    } else {
      std::cout << "Test maxlevel lake-small-65535 passed!" << '\n';
    }
  }

  [[maybe_unused]] void test_maxlevelLakeSmall255() {
    std::string const input1     = "../../input/lake-small.ppm";
    std::string const input2     = "../../output/maxlevel/lake-small-255.ppm";
    constexpr size_t newMaxColor = 255;
    PPMMetadata const metadata1  = loadMetadata(input1);
    PPMMetadata const metadata2  = loadMetadata(input2);
    auto const image1            = std::make_unique<ImageSOA_8bit>(metadata1);
    auto const image2            = std::make_unique<ImageSOA_8bit>(metadata2);
    image1->loadData(input1);
    image2->loadData(input2);
    image1->maxLevel(newMaxColor);
    if (!(*image1 == *image2)) {
      std::cerr << "Test maxlevel lake-small-255 failed!" << '\n';
    } else {
      std::cout << "Test maxlevel lake-small-255 passed!" << '\n';
    }
  }

  [[maybe_unused]] void test_maxlevelLakeSmall65535() {
    std::string const input1     = "../../input/lake-small.ppm";
    std::string const input2     = "../../output/maxlevel/lake-small-65535.ppm";
    constexpr size_t newMaxColor = 65535;
    PPMMetadata const metadata1  = loadMetadata(input1);
    PPMMetadata const metadata2  = loadMetadata(input2);
    auto const image1            = std::make_unique<ImageSOA_8bit>(metadata1);
    auto const image2            = std::make_unique<ImageSOA_16bit>(metadata2);
    image1->loadData(input1);
    image2->loadData(input2);
    std::unique_ptr<ImageSOA_16bit> const image3 = image1->maxLevelChangeChannelSize(newMaxColor);
    if (!(*image3 == *image2)) {
      std::cerr << "Test maxlevel lake-small-65535 failed!" << '\n';
    } else {
      std::cout << "Test maxlevel lake-small-65535 passed!" << '\n';
    }
  }

  [[maybe_unused]] void test_maxlevel(std::string & time) {
    time = test_wrapper(test_maxlevelDeerSmall100);
    std::cout << "Test maxlevel deer-small-100 finished in:" << time << '\n';
    time = test_wrapper(test_maxlevelDeerSmall255);
    std::cout << "Test maxlevel deer-small-255 finished in:" << time << '\n';
    time = test_wrapper(test_maxlevelDeerSmall1000);
    std::cout << "Test maxlevel deer-small-1000 finished in:" << time << '\n';
    time = test_wrapper(test_maxlevelDeerSmall65535);
    std::cout << "Test maxlevel deer-small-65535 finished in:" << time << '\n';

    time = test_wrapper(test_maxlevelLakeSmall255);
    std::cout << "Test maxlevel lake-small-255 finished in:" << time << '\n';
    time = test_wrapper(test_maxlevelLakeSmall65535);
    std::cout << "Test maxlevel lake-small-65535 finished in:" << time << '\n';
  }

  [[maybe_unused]] void test_resize(std::string & time) {
    time = test_wrapper(test_resizeDeerLarge100);
    std::cout << "Test resize deer-large-100 finished in:" << time << '\n';
    time = test_wrapper(test_resizeDeerLarge1000);
    std::cout << "Test resize deer-large-1000 finished in:" << time << '\n';
    time = test_wrapper(test_resizeDeerSmall100);
    std::cout << "Test resize deer-small-100 finished in:" << time << '\n';
    time = test_wrapper(test_resizeDeerSmall1000);
    std::cout << "Test resize deer-small-1000 finished in:" << time << '\n';

    time = test_wrapper(test_resizeLakeLarge100);
    std::cout << "Test resize lake-large-100 finished in:" << time << '\n';
    time = test_wrapper(test_resizeLakeLarge1000);
    std::cout << "Test resize lake-large-1000 finished in:" << time << '\n';
    time = test_wrapper(test_resizeLakeSmall100);
    std::cout << "Test resize lake-small-100 finished in:" << time << '\n';
    time = test_wrapper(test_resizeLakeSmall1000);
    std::cout << "Test resize lake-small-1000 finished in:" << time << '\n';
  }

  [[maybe_unused]] void test_cutfreq(std::string & time) {
    time = test_wrapper(test_cutFreqLake100k);
    std::cout << "Test cutfreq lake-100K finished in:" << time << '\n';
    time = test_wrapper(test_cutFreqLake162k);
    std::cout << "Test cutfreq lake-162K finished in:" << time << '\n';
  }
}  // namespace

int main() {
  std::filesystem::path const cwd = std::filesystem::current_path();
  test_comparator();
  std::cout << "Current working directory: " << cwd << '\n';
  std::string time;
  //test_cutfreq(time);

  test_resize(time);

  test_maxlevel(time);
}