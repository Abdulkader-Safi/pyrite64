/**
* @copyright 2026 - Max Bebök
* @license MIT
*/
#pragma once
#include "mesh.h"

namespace P64::Coll
{
  class Attach
  {
    private:
      fm_vec3_t refPos{};
      fm_vec3_t refPosLocal{};

      uint16_t refId{};
      uint16_t lastRefId{};

    public:
    Attach() = default;

    fm_vec3_t update(const fm_vec3_t &ownPos);
    void setReference(const MeshInstance *meshInst);
  };
}
