/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "input.h"

void Runtime::Input::init() {}
void Runtime::Input::shutdown() {}

void Runtime::Input::update()
{
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_QUIT:
        quitRequested = true;
        break;
      case SDL_EVENT_KEY_DOWN:
        if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
          quitRequested = true;
        }
        break;
      default:
        break;
    }
  }
}

bool Runtime::Input::isKeyDown(SDL_Scancode key) const
{
  const bool *state = SDL_GetKeyboardState(nullptr);
  return state[key];
}

bool Runtime::Input::isKeyPressed(SDL_Scancode /*key*/) const
{
  // TODO: track previous frame state for edge detection
  return false;
}

float Runtime::Input::getGamepadAxis(SDL_GamepadAxis /*axis*/) const
{
  // TODO: implement gamepad support
  return 0.0f;
}

bool Runtime::Input::isButtonDown(SDL_GamepadButton /*button*/) const
{
  // TODO: implement gamepad support
  return false;
}

bool Runtime::Input::isButtonPressed(SDL_GamepadButton /*button*/) const
{
  // TODO: implement gamepad support
  return false;
}
