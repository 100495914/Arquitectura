//
// Created by diego on 24/10/24.
//

#ifndef PPM_METADATA_HPP
#define PPM_METADATA_HPP

#include <string>

struct PPMMetadata {
  std::string magicNumber;
  int width;
  int height;
  int maxColorValue;
};

#endif // PPM_METADATA_HPP
