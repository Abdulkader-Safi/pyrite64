/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "audio.h"

bool Runtime::Audio::init()
{
  // TODO: Phase 4 — SDL3 audio device + stream init
  return true;
}

void Runtime::Audio::shutdown()
{
  // TODO: Phase 4
}

void Runtime::Audio::playSound(const std::string &/*path*/) {}
void Runtime::Audio::playMusic(const std::string &/*path*/) {}
void Runtime::Audio::stopAll() {}
