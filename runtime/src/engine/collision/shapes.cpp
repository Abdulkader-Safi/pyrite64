/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "engine/collision/shapes.h"

bool P64::Coll::AABB::vsAABB(const Coll::AABB &other) const {
  return (max.v[0] >= other.min.v[0])
      && (max.v[1] >= other.min.v[1])
      && (max.v[2] >= other.min.v[2])
      && (min.v[0] <= other.max.v[0])
      && (min.v[1] <= other.max.v[1])
      && (min.v[2] <= other.max.v[2]);
}

bool P64::Coll::AABB::vsRay(const fm_vec3_t &pos, const fm_vec3_t &dir) const
{
  constexpr float DEF_MAX_FLOAT = 10000.0f;
  auto invDir = fm_vec3_t{DEF_MAX_FLOAT, DEF_MAX_FLOAT, DEF_MAX_FLOAT};
  if(dir.x != 0)invDir.x = 1.0f / dir.x;
  if(dir.y != 0)invDir.y = 1.0f / dir.y;
  if(dir.z != 0)invDir.z = 1.0f / dir.z;
  auto vecMin = (fm_vec3_t{(float)min.v[0], (float)min.v[1], (float)min.v[2]} - pos) * invDir;
  auto vecMax = (fm_vec3_t{(float)max.v[0], (float)max.v[1], (float)max.v[2]} - pos) * invDir;

  fm_vec3_t t1{
    fminf(vecMin.x, vecMax.x),
    fminf(vecMin.y, vecMax.y),
    fminf(vecMin.z, vecMax.z)
  };
  fm_vec3_t t2{
    fmaxf(vecMin.x, vecMax.x),
    fmaxf(vecMin.y, vecMax.y),
    fmaxf(vecMin.z, vecMax.z)
  };

  float near = fmaxf(fmaxf(t1.x, t1.y), t1.z);
  float far = fminf(fminf(t2.x, t2.y), t2.z);

  return (far >= near ? (near < 0.0f ? far : near) : -1.0f) >= 0.0f;
}

bool P64::Coll::AABB::vsPoint(const IVec3 &pos) const {
  return (pos.v[0] >= min.v[0])
      && (pos.v[1] >= min.v[1])
      && (pos.v[2] >= min.v[2])
      && (pos.v[0] <= max.v[0])
      && (pos.v[1] <= max.v[1])
      && (pos.v[2] <= max.v[2]);
}
