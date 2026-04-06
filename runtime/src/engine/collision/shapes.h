/**
* @copyright 2023 - Max Bebök
* @license MIT
*/
#pragma once
#include <functional>
#include "engine/lib/math.h"
#include "flags.h"

namespace P64
{
  class Object;
}

namespace P64::Coll
{
  struct MeshInstance;
  struct BCS;

  struct IVec3 {
    int16_t v[3]{};
  };

  struct AABB
  {
    IVec3 min{};
    IVec3 max{};

    bool vsAABB(const AABB &other) const;
    bool vsRay(const fm_vec3_t &pos, const fm_vec3_t &dir) const;
    bool vsPoint(const IVec3 &pos) const;
  };
  static_assert(sizeof(AABB) == (6 * sizeof(int16_t)));

  struct BCS
  {
    fm_vec3_t center{};
    fm_vec3_t halfExtend{};
    fm_vec3_t velocity{};
    fm_vec3_t parentOffset{};

    P64::Object *obj{};

    uint8_t maskRead{0};
    uint8_t maskWrite{0};
    uint8_t flags{0};
    uint8_t hitTriTypes{0};

    [[nodiscard]] constexpr float getRadius() const {
      return halfExtend.y;
    }
    [[nodiscard]] constexpr float getRadius2() const {
      return halfExtend.y * halfExtend.y;
    }

    [[nodiscard]] constexpr bool isTrigger() const {
      return (flags & BCSFlags::TRIGGER);
    }

    [[nodiscard]] constexpr bool isSolid() const {
      return !isTrigger();
    }

    [[nodiscard]] constexpr bool isFixed() const {
      return flags & BCSFlags::FIXED_XYZ;
    }

    [[nodiscard]] fm_vec3_t getMinAABB() const {
      return center - halfExtend;
    }
    [[nodiscard]] fm_vec3_t getMaxAABB() const {
      return center + halfExtend;
    }

    BCS operator*(float scale) const {
      return {
        .center = center * scale,
        .halfExtend = halfExtend * scale
      };
    }

    AABB toAABB() const {
      auto mn = center - halfExtend;
      auto mx = center + halfExtend;
      return {
        .min = {{ (int16_t)mn.x, (int16_t)mn.y, (int16_t)mn.z }},
        .max = {{ (int16_t)mx.x, (int16_t)mx.y, (int16_t)mx.z }}
      };
    }
  };

  struct CollInfo
  {
    fm_vec3_t penetration{};
    fm_vec3_t floorWallAngle{};
    MeshInstance* meshInstance{};
    int collCount{};
  };

  struct RaycastRes {
    fm_vec3_t hitPos{};
    fm_vec3_t normal{};
    uint32_t flags{};

    [[nodiscard]] bool hasResult() const {
      return flags != 0;
    }
  };

  struct Triangle
  {
    fm_vec3_t normal{};
    fm_vec3_t* v[3]{};
    AABB aabb{};
  };

  struct Triangle2D {
    fm_vec2_t v[3]{};
  };

  struct CollEvent
  {
    BCS* selfBCS{};
    BCS* otherBCS{};

    MeshInstance* selfMesh{};
    MeshInstance* otherMesh{};
  };
}
