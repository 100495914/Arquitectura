//
// Created by diego on 20/10/24.
//
#include "imgsoa/imagesoa.hpp"

#include <chrono>
#include <filesystem>
#include <iostream>

namespace {

  std::chrono::duration<long> test_wrapper(void (*func)()) {
    auto const start = std::chrono::high_resolution_clock::now();
    func();
    auto const end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(end - start);
  }

  void test_comparator() {
    std::string const input1    = "../../input/lake-large.ppm";
    PPMMetadata const metadata1 = loadMetadata(input1);
    auto const image1           = std::make_unique<ImageSOA_8bit>(metadata1);
    auto const image2           = std::make_unique<ImageSOA_8bit>(metadata1);
    image1->loadData(input1);
    image2->loadData(input1);
    if (*image1 != *image2) {
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
    if (*image1 != *image2) {
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
    if (*image1 != *image2) {
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
    if (*image1 != *image2) {
      std::cerr << "Test cutFreq deer-large-100 failed!" << '\n';
    } else {
      std::cout << "Test cutFreq deer-large-100 passed!" << '\n';
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
    if (*image1 != *image2) {
      std::cerr << "Test cutFreq deer-large-1000 failed!" << '\n';
    } else {
      std::cout << "Test cutFreq deer-large-1000 passed!" << '\n';
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
    if (*image1 != *image2) {
      std::cerr << "Test cutFreq deer-small-100 failed!" << '\n';
    } else {
      std::cout << "Test cutFreq deer-small-100 passed!" << '\n';
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
    if (*image1 != *image2) {
      std::cerr << "Test cutFreq deer-small-1000 failed!" << '\n';
    } else {
      std::cout << "Test cutFreq deer-small-1000 passed!" << '\n';
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
    if (*image1 != *image2) {
      std::cerr << "Test cutFreq lake-large-100 failed!" << '\n';
    } else {
      std::cout << "Test cutFreq lake-large-100 passed!" << '\n';
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
    if (*image1 != *image2) {
      std::cerr << "Test cutFreq lake-large-1000 failed!" << '\n';
    } else {
      std::cout << "Test cutFreq lake-large-1000 passed!" << '\n';
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
    if (*image1 != *image2) {
      std::cerr << "Test cutFreq lake-small-100 failed!" << '\n';
    } else {
      std::cout << "Test cutFreq lake-small-100 passed!" << '\n';
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
    if (*image1 != *image2) {
      std::cerr << "Test cutFreq lake-small-1000 failed!" << '\n';
    } else {
      std::cout << "Test cutFreq lake-small-1000 passed!" << '\n';
    }
  }
}  // namespace

int main() {
  std::filesystem::path const cwd = std::filesystem::current_path();
  test_comparator();
  std::cout << "Current working directory: " << cwd << '\n';
  std::chrono::duration<long> time{};

  time = test_wrapper(test_resizeDeerLarge100);
  std::cout << "Test deer-large-100 finished in:" << time << '\n';
  time = test_wrapper(test_resizeDeerLarge1000);
  std::cout << "Test deer-large-1000 finished in:" << time << '\n';
  time = test_wrapper(test_resizeDeerSmall100);
  std::cout << "Test deer-small-100 finished in:" << time << '\n';
  time = test_wrapper(test_resizeDeerSmall1000);
  std::cout << "Test deer-small-1000 finished in:" << time << '\n';

  time = test_wrapper(test_resizeLakeLarge100);
  std::cout << "Test lake-large-100 finished in:" << time << '\n';
  time = test_wrapper(test_resizeLakeLarge1000);
  std::cout << "Test lake-large-1000 finished in:" << time << '\n';
  time = test_wrapper(test_resizeLakeSmall100);
  std::cout << "Test lake-small-100 finished in:" << time << '\n';
  time = test_wrapper(test_resizeLakeSmall1000);
  std::cout << "Test lake-small-1000 finished in:" << time << '\n';
}