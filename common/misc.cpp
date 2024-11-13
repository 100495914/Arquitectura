//
// Created by Alberto on 13/11/2024.
//

#include "imgsoa/imagesoa.hpp"

int numberInXbitRange(uint const number) {
  if (number <= MAX_8BIT_VALUE) {
    constexpr uint magic_number_8 = 8;
    return magic_number_8;
  }
  if (number <= MAX_16BIT_VALUE) {
    constexpr uint magic_number_16 = 16;
    return magic_number_16;
  }
  return -1;
}
