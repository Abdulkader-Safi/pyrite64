/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#pragma once
#include <vector>
#include <string>
#include "shapes.h"

namespace P64
{
  class Object;
}

namespace P64::Coll
{
  struct BVH;

  struct Mesh
  {
    uint32_t triCount{};
    uint32_t vertCount{};
    float collScale{};
    fm_vec3_t *verts{};
    IVec3 *normals{};
    BVH* bvh{};
    int16_t indices[];

    [[nodiscard]] CollInfo vsSphere(const BCS &sphere, const Triangle& triangle) const;
    [[nodiscard]] CollInfo vsBox(const BCS &box, const Triangle& triangle) const;
    [[nodiscard]] RaycastRes vsRay(const fm_vec3_t &pos, const fm_vec3_t &dir, const Triangle& triangle) const;

    static Mesh* load(void* rawData);
  };

  struct MeshInstance {
    Mesh *mesh{};
    P64::Object* object{};

    fm_vec3_t invScale{};
    fm_quat_t invRot{};

    fm_vec3_t intoLocalSpace(const fm_vec3_t &p) const;
    fm_vec3_t outOfLocalSpace(const fm_vec3_t &p) const;
    void update();
  };
}
