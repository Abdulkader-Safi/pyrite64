/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include "engine/lib/math.h"

namespace P64
{
  struct LightData {
    fm_vec3_t dir{};
    fm_vec3_t color{};
  };

  class Lighting
  {
    private:
      LightData ambientLight{};
      LightData dirLights[2]{};
      uint32_t dirLightCount{0};

    public:
      void reset() {
        ambientLight = {};
        dirLightCount = 0;
      }

      void addAmbientLight(const fm_vec3_t &color) {
        ambientLight.color = color;
      }

      void addDirLight(const fm_vec3_t &dir, const fm_vec3_t &color) {
        if(dirLightCount < 2) {
          dirLights[dirLightCount].dir = dir;
          dirLights[dirLightCount].color = color;
          dirLightCount++;
        }
      }

      void apply() {
        // TODO: Phase 3 — apply lights to runtime renderer uniforms
      }

      [[nodiscard]] const LightData& getAmbientLight() const { return ambientLight; }
      [[nodiscard]] uint32_t getDirLightCount() const { return dirLightCount; }
      [[nodiscard]] const LightData& getDirLight(uint32_t idx) const { return dirLights[idx]; }
  };
}
