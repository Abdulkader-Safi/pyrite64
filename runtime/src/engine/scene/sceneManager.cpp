/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "engine/scene/sceneManager.h"
#include "engine/scene/scene.h"

namespace {
  P64::Scene *currentScene{nullptr};
  uint16_t pendingSceneId{0};
  bool hasPendingScene{false};
}

void P64::SceneManager::load(uint16_t newSceneId)
{
  pendingSceneId = newSceneId;
  hasPendingScene = true;
}

P64::Scene& P64::SceneManager::getCurrent()
{
  return *currentScene;
}
