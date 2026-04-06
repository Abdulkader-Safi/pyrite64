/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "filesystem.h"
#include <SDL3/SDL.h>

void Runtime::Filesystem::init(const std::string &basePath)
{
  base = basePath;
  if (!base.empty() && base.back() != '/') {
    base += '/';
  }
}

std::vector<uint8_t> Runtime::Filesystem::loadFile(const std::string &relativePath) const
{
  std::string fullPath = base + relativePath;
  size_t size = 0;
  void *data = SDL_LoadFile(fullPath.c_str(), &size);
  if (!data) {
    return {};
  }
  std::vector<uint8_t> result(static_cast<uint8_t*>(data), static_cast<uint8_t*>(data) + size);
  SDL_free(data);
  return result;
}
