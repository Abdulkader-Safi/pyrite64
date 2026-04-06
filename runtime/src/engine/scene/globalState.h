/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <cstdint>

namespace P64 {
  struct GlobalState
  {
    uint32_t screenSize[2]{};
  };

  extern GlobalState state;
}
