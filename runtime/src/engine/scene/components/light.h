/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <cstdint>
#include "engine/lib/math.h"

namespace P64::Comp
{
  struct Light {
    static constexpr uint8_t ID = 2;
    fm_vec3_t dir{};
    fm_vec3_t color{};
    uint8_t type{};
  };
}
