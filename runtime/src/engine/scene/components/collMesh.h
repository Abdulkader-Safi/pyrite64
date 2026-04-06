/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <cstdint>
#include "engine/collision/mesh.h"

namespace P64::Comp
{
  struct CollMesh {
    static constexpr uint8_t ID = 4;
    Coll::MeshInstance meshInstance{};
  };
}
