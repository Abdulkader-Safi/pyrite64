/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "engine/collision/resolver.h"

using namespace P64;

namespace
{
  namespace TriType = Coll::TriType;

  inline float copySign(float val, float sign) {
    return sign < 0 ? -val : val;
  }

  bool separateBCS(Coll::BCS &bcsA, Coll::BCS &bcsB, const fm_vec3_t &dir, float dist2, float distTotal)
  {
    if(dist2 > (distTotal*distTotal))return false;

    if(bcsA.isSolid() && bcsB.isSolid())
    {
      float dist = sqrtf(fmaxf(dist2, 0.0001f));
      auto dirNorm = (dir /  dist);
      auto pen = distTotal - dist;

      bool isFixedA = bcsA.flags & Coll::BCSFlags::FIXED_XYZ;
      bool isFixedB = bcsB.flags & Coll::BCSFlags::FIXED_XYZ;
      if(!isFixedA || !isFixedB) {

        float interp = bcsA.halfExtend.y / (bcsA.halfExtend.y + bcsB.halfExtend.y);
        if(isFixedA)interp = 1.0f;
        if(isFixedB)interp = 0.0f;

        if(!isFixedA)bcsA.center = bcsA.center + dirNorm * (pen * (1.0f - interp));
        if(!isFixedB)bcsB.center = bcsB.center - dirNorm * (pen * interp);

        if(dirNorm.y > 0.9f) {
          bcsA.hitTriTypes |= TriType::FLOOR;
          bcsA.velocity.y = 0.0f;
        } else {
          bcsA.hitTriTypes |= TriType::WALL;
        }
      }

      bcsA.hitTriTypes |= TriType::BCS;
      bcsB.hitTriTypes |= TriType::BCS;
    }
    return true;
  }
}

bool Coll::sphereVsSphere(Coll::BCS &collA, Coll::BCS &collB)
{
  fm_vec3_t dir = collA.center - collB.center;
  auto dist2 = glm::length2(dir);
  float radSum = collA.getRadius() + collB.getRadius();
  return separateBCS(collA, collB, dir, dist2, radSum);
}

bool Coll::sphereVsBox(Coll::BCS &sphere, Coll::BCS &box) {
  auto const closestPoint = Math::max(
    box.getMinAABB(),
    Math::min(sphere.center, box.getMaxAABB())
  );
  auto dir = sphere.center - closestPoint;
  float dist2 = glm::length2(dir);
  return separateBCS(sphere, box, dir, dist2, sphere.getRadius());
}

bool Coll::boxVsBox(Coll::BCS &collA, Coll::BCS &collB) {
  auto combExtend = collA.halfExtend + collB.halfExtend;
  auto posDiff = collB.center - collA.center;
  auto posDiffAbs = Math::abs(posDiff);

  if(posDiffAbs.x > combExtend.x)return false;
  if(posDiffAbs.y > combExtend.y)return false;
  if(posDiffAbs.z > combExtend.z)return false;

  bool solidA = collA.isSolid();
  bool solidB = collB.isSolid();

  bool isFixedA = collA.flags & Coll::BCSFlags::FIXED_XYZ;
  bool isFixedB = collB.flags & Coll::BCSFlags::FIXED_XYZ;

  if(solidA && solidB && (!isFixedA || !isFixedB))
  {
    float interp = collA.halfExtend.y / (collA.halfExtend.y + collB.halfExtend.y);
    if(isFixedA)interp = 1.0f;
    if(isFixedB)interp = 0.0f;

    fm_vec3_t penDiff = combExtend - posDiffAbs;
    float min = Math::min(penDiff);

    if(min == penDiff.x){
        float pen = copySign(penDiff.x, posDiff.x);
        collA.center.x = collA.center.x - pen * (1.0f - interp);
        collB.center.x = collB.center.x + pen * (interp);
    }
    else if(min == penDiff.y){
        float pen = copySign(penDiff.y, posDiff.y);
        collA.center.y = collA.center.y - pen * (1.0f - interp);
        collB.center.y = collB.center.y + pen * (interp);
    }
    else if(min == penDiff.z){
        float pen = copySign(penDiff.z, posDiff.z);
        collA.center.z = collA.center.z - pen * (1.0f - interp);
        collB.center.z = collB.center.z + pen * (interp);
    }
  }

  collA.hitTriTypes |= TriType::BCS;
  collB.hitTriTypes |= TriType::BCS;

  return true;
}
