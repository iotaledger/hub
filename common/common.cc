/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <glog/logging.h>

namespace common {

std::string readFile(const std::string& fileName) {
  std::ifstream ifs(fileName.c_str());

  if (!ifs.good()) {
    LOG(FATAL) << "File: " << fileName << " does not exist.";
  }

  std::stringstream buffer;

  buffer << ifs.rdbuf();
  return buffer.str();
}
}  // namespace common
