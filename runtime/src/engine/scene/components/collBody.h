/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <cstdint>
#include "engine/collision/shapes.h"

namespace P64::Comp
{
  struct CollBody {
    static constexpr uint8_t ID = 5;
    Coll::BCS bcs{};
    fm_vec3_t origScale{};
  };
}
