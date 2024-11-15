//
// Created by diego on 15/11/24.
//

#ifndef GET_PPM_METADATA_TEST_HPP
#define GET_PPM_METADATA_TEST_HPP

#include <string>
#include <istream>
#include "../common/PPMMetadata.hpp"

// Declaración de la función auxiliar para obtener los metadatos desde un flujo de entrada
PPMMetadata getPPMMetadataFromStream(std::istream& file);

#endif // GET_PPM_METADATA_TEST_HPP
