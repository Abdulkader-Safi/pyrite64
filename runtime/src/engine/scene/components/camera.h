/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <cstdint>
#include "engine/lib/math.h"

namespace P64::Comp
{
  struct Camera {
    static constexpr uint8_t ID = 3;
    // TODO: Phase 3 — GLM-based view/projection
    fm_mat4_t viewMatrix{1.0f};
    fm_mat4_t projMatrix{1.0f};
  };
}
