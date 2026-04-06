/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <cstdint>

namespace P64
{
  class Scene;
}

namespace P64::SceneManager
{
  void load(uint16_t newSceneId);
  Scene &getCurrent();
}
