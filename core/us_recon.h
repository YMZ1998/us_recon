#pragma once
#include <vector>


namespace us {
class UsRecon {
 public:
  UsRecon() {}
  ~UsRecon() {}

 private:
  std::vector<std::pair<unsigned int, unsigned int>> coords_;
};
}  // namespace us