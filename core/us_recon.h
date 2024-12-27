#pragma once
#include <iostream>
#include <vector>

#include "images.h"

namespace us {
class UsRecon {
 public:
  UsRecon() {}
  ~UsRecon() {}

 private:
  std::vector<std::pair<unsigned int, unsigned int>> coords_;
};
}  // namespace us