/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace Runtime
{
  class Filesystem
  {
    public:
      void init(const std::string &basePath);

      // Load a file relative to the base path, returns contents or empty on failure
      std::vector<uint8_t> loadFile(const std::string &relativePath) const;

      const std::string& getBasePath() const { return base; }

    private:
      std::string base{};
  };
}
