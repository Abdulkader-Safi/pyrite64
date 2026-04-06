/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "window.h"
#include <cstdio>

#ifdef HAS_SHADER_CROSS
  #include "SDL3_shadercross/SDL_shadercross.h"
#endif

bool Runtime::Window::init(int width, int height, const std::string &title)
{
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
    fprintf(stderr, "Error: SDL_Init(): %s\n", SDL_GetError());
    return false;
  }

#ifdef HAS_SHADER_CROSS
  if (!SDL_ShaderCross_Init()) {
    fprintf(stderr, "Error: SDL_ShaderCross_Init(): %s\n", SDL_GetError());
    return false;
  }
#endif

  window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_RESIZABLE);
  if (!window) {
    fprintf(stderr, "Error: SDL_CreateWindow(): %s\n", SDL_GetError());
    return false;
  }

  gpu = SDL_CreateGPUDevice(
    SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_DXIL,
    false, nullptr
  );
  if (!gpu) {
    fprintf(stderr, "Error: SDL_CreateGPUDevice(): %s\n", SDL_GetError());
    return false;
  }

  auto props = SDL_GetGPUDeviceProperties(gpu);
  auto gpuName = SDL_GetStringProperty(props, SDL_PROP_GPU_DEVICE_NAME_STRING, "");
  auto gpuDriver = SDL_GetStringProperty(props, SDL_PROP_GPU_DEVICE_DRIVER_NAME_STRING, "");
  printf("GPU: %s | %s\n", gpuName, gpuDriver);

  if (!SDL_ClaimWindowForGPUDevice(gpu, window)) {
    fprintf(stderr, "Error: SDL_ClaimWindowForGPUDevice(): %s\n", SDL_GetError());
    return false;
  }

  SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;
  if (SDL_WindowSupportsGPUPresentMode(gpu, window, SDL_GPU_PRESENTMODE_IMMEDIATE)) {
    presentMode = SDL_GPU_PRESENTMODE_IMMEDIATE;
    vsync = false;
  } else {
    vsync = true;
  }
  SDL_SetGPUSwapchainParameters(gpu, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, presentMode);

  return true;
}

void Runtime::Window::shutdown()
{
  if (gpu) {
    SDL_ReleaseWindowFromGPUDevice(gpu, window);
    SDL_DestroyGPUDevice(gpu);
    gpu = nullptr;
  }
  if (window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }

#ifdef HAS_SHADER_CROSS
  SDL_ShaderCross_Quit();
#endif

  SDL_Quit();
}

SDL_GPUCommandBuffer* Runtime::Window::beginFrame()
{
  cmdBuf = SDL_AcquireGPUCommandBuffer(gpu);
  swapTex = nullptr;
  SDL_WaitAndAcquireGPUSwapchainTexture(cmdBuf, window, &swapTex, nullptr, nullptr);
  return cmdBuf;
}

void Runtime::Window::endFrame()
{
  SDL_SubmitGPUCommandBuffer(cmdBuf);
  cmdBuf = nullptr;
  swapTex = nullptr;
}
