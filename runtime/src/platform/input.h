/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <SDL3/SDL.h>

namespace Runtime
{
  class Input
  {
    public:
      void init();
      void shutdown();

      // Call once per frame before processing game logic
      void update();

      // Keyboard
      bool isKeyDown(SDL_Scancode key) const;
      bool isKeyPressed(SDL_Scancode key) const;

      // Gamepad
      float getGamepadAxis(SDL_GamepadAxis axis) const;
      bool isButtonDown(SDL_GamepadButton button) const;
      bool isButtonPressed(SDL_GamepadButton button) const;

      // Window events
      bool wasQuitRequested() const { return quitRequested; }

    private:
      bool quitRequested{false};
  };
}
