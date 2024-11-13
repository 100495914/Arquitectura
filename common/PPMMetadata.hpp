//
// Created by diego on 24/10/24.
//

#ifndef PPM_METADATA_HPP
#define PPM_METADATA_HPP

#include <string>

struct PPMMetadata {
  std::string magicNumber;
  int width = 0;
  int height = 0;
  int maxColorValue = 0;
};

PPMMetadata getPPMMetadata(const std::string& filename);

#endif // PPM_METADATA_HPP
