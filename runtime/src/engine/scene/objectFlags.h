/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <cstdint>

namespace P64::ObjectFlags
{
  constexpr uint16_t SELF_ACTIVE    = 1 << 0;
  constexpr uint16_t PARENTS_ACTIVE = 1 << 1;
  constexpr uint16_t HAS_CHILDREN   = 1 << 2;
  constexpr uint16_t PENDING_REMOVE = 1 << 4;
  constexpr uint16_t IS_CULLED      = 1 << 5;

  constexpr uint16_t ACTIVE = SELF_ACTIVE | PARENTS_ACTIVE;
}
