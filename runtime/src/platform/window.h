/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <string>
#include <SDL3/SDL.h>

namespace Runtime
{
  class Window
  {
    private:
      SDL_Window* window{nullptr};
      SDL_GPUDevice* gpu{nullptr};
      SDL_GPUCommandBuffer* cmdBuf{nullptr};
      SDL_GPUTexture* swapTex{nullptr};
      bool vsync{false};

    public:
      bool init(int width, int height, const std::string &title);
      void shutdown();

      SDL_Window* getWindow() const { return window; }
      SDL_GPUDevice* getGPU() const { return gpu; }
      bool isVSync() const { return vsync; }

      // Frame lifecycle
      SDL_GPUCommandBuffer* beginFrame();
      SDL_GPUTexture* getSwapchainTexture() const { return swapTex; }
      void endFrame();
  };
}
