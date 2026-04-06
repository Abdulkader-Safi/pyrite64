/**
 * @copyright 2025 - Max Bebök
 * @license MIT
 */
#include <SDL3/SDL.h>
#include <cstdio>

#include "platform/input.h"
#include "platform/window.h"

int main(int /*argc*/, char ** /*argv*/) {
  Runtime::Window window;
  if (!window.init(1280, 720, "Pyrite64 Game")) {
    return -1;
  }

  Runtime::Input input;
  input.init();

  bool running = true;
  while (running) {
    input.update();
    if (input.wasQuitRequested()) {
      running = false;
      break;
    }

    auto *cmdBuf = window.beginFrame();
    auto *swapTex = window.getSwapchainTexture();

    if (!swapTex) {
      window.endFrame();
      continue;
    }

    // Clear to a dark blue-gray
    SDL_GPUColorTargetInfo colorTarget{};
    colorTarget.texture = swapTex;
    colorTarget.clear_color = {0.08f, 0.08f, 0.12f, 1.0f};
    colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;

    auto *renderPass = SDL_BeginGPURenderPass(cmdBuf, &colorTarget, 1, nullptr);
    SDL_EndGPURenderPass(renderPass);

    window.endFrame();
  }

  input.shutdown();
  window.shutdown();
  return 0;
}
