/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <string>
#include <cstdint>

namespace Runtime
{
  class Audio
  {
    public:
      bool init();
      void shutdown();

      // TODO: Phase 4 — full audio manager with slots, handles, volume control
      void playSound(const std::string &path);
      void playMusic(const std::string &path);
      void stopAll();
  };
}
