/**
 * @author Max Bebök
 * @license TBD
 */
#include "engine/collision/scene.h"
#include "engine/scene/scene.h"

#include "engine/collision/bvh.h"
#include "engine/collision/resolver.h"
#include "engine/lib/logger.h"
#include "engine/scene/sceneManager.h"

#include <chrono>

namespace
{
  constexpr float MIN_PENETRATION = 0.00004f;
  constexpr float FLOOR_ANGLE = 0.4f;

  constexpr bool isFloor(const P64::Coll::IVec3 &normal) {
    return normal.v[1] > (int16_t)(0x7FFF * FLOOR_ANGLE);
  }
  constexpr bool isFloor(const fm_vec3_t &normal) {
    return normal.y > FLOOR_ANGLE;
  }
  constexpr bool isFloor(float normY) {
    return normY > FLOOR_ANGLE;
  }

  uint64_t getTicks() {
    return std::chrono::steady_clock::now().time_since_epoch().count();
  }
}

P64::Coll::CollInfo P64::Coll::Scene::vsBCS(BCS &bcs, const fm_vec3_t &velocity, float deltaTime) {
  float len = glm::length(velocity) * deltaTime;

  bool isBox = bcs.flags & BCSFlags::SHAPE_BOX;

  int steps = (int)(len * 1.5f);
  steps = P64::Math::clamp(steps, 1, 8);

  auto velocityStep = velocity * (deltaTime / steps);

  P64::Coll::CollInfo res{};
  P64::Coll::BVHResult bvhRes{};

  for(int s=0; s<steps; ++s)
  {
    bcs.center = bcs.center + velocityStep;

    for(auto meshInst : meshes)
    {
      auto &mesh = *meshInst->mesh;

      auto bcsLocal = bcs;
      bcsLocal.center = meshInst->intoLocalSpace(bcs.center);
      bcsLocal.halfExtend *= meshInst->invScale;

      auto ticksBvhStart = getTicks();
      bvhRes.reset();
      mesh.bvh->vsBCS(bcsLocal, bvhRes);

      ticksBVH += getTicks() - ticksBvhStart;
      if(bvhRes.count >= P64::Coll::MAX_RESULT_COUNT-1) {
        P64::Log::error("BVH result count exceeded max limit (%d)\n", bvhRes.count);
      }

      for(int b=0; b<bvhRes.count; ++b) {
        uint32_t t = bvhRes.triIndex[b];

        int idxA = mesh.indices[t*3];
        int idxB = mesh.indices[t*3+1];
        int idxC = mesh.indices[t*3+2];
        auto &norm = mesh.normals[t];

        Triangle tri{
          .normal = {
           (float)norm.v[0] * (1.0f / 32767.0f),
           (float)norm.v[1] * (1.0f / 32767.0f),
           (float)norm.v[2] * (1.0f / 32767.0f)
          },
          .v = {&mesh.verts[idxA], &mesh.verts[idxB], &mesh.verts[idxC]}
        };

        auto collInfo = isBox
          ? mesh.vsBox(bcsLocal, tri)
          : mesh.vsSphere(bcsLocal, tri);

        if(collInfo.collCount)
        {
          float penLen2 = glm::length2(collInfo.penetration);
          if(penLen2 < MIN_PENETRATION)continue;

          ++res.collCount;
          res.penetration = res.penetration + collInfo.penetration;
          res.meshInstance = meshInst;

          collInfo.floorWallAngle = meshInst->object->rot * collInfo.floorWallAngle;

          bool hitFloor = isFloor(collInfo.floorWallAngle.y);
          bcs.hitTriTypes |= hitFloor ? TriType::FLOOR : TriType::WALL;
          if(hitFloor) {
            res.floorWallAngle.y = collInfo.floorWallAngle.y;
          } else {
            res.floorWallAngle.x = collInfo.floorWallAngle.x;
            res.floorWallAngle.z = collInfo.floorWallAngle.z;
          }

          bcsLocal.center -= collInfo.penetration;
        }
      }

      bcs.center = meshInst->outOfLocalSpace(bcsLocal.center);
    }
  }

  return res;
}

fm_vec3_t P64::Coll::MeshInstance::intoLocalSpace(const fm_vec3_t &p) const {
  auto res = (p - object->pos);
  return invRot * res * invScale;
}
fm_vec3_t P64::Coll::MeshInstance::outOfLocalSpace(const fm_vec3_t &p) const {
  return object->rot * (p * object->scale) + object->pos;
}

void P64::Coll::MeshInstance::update()
{
  invScale = fm_vec3_t{
    1.0f / object->scale.x,
    1.0f / object->scale.y,
    1.0f / object->scale.z,
  };
  invRot = glm::inverse(object->rot);
}

void P64::Coll::Scene::update(float deltaTime)
{
  uint64_t ticksStart = getTicks();
  auto &gameScene = P64::SceneManager::getCurrent();

  for(auto &inst : meshes) {
    inst->update();
  }

  for(auto sp : collBCS) {
    sp->hitTriTypes = 0;
  }

  for(uint32_t s=0; s < collBCS.size(); ++s) {
    auto &bcsA = collBCS[s];

    bool checkColl = bcsA->isSolid() && !bcsA->isFixed();

    if(checkColl) {
      auto res = vsBCS(*bcsA, bcsA->velocity, deltaTime);
      if(res.collCount)
      {
        bool hitFloor = bcsA->hitTriTypes & TriType::FLOOR;
        if(bcsA->flags & BCSFlags::BOUNCY) {
          bcsA->velocity = bcsA->velocity - res.floorWallAngle * 2.0f * glm::dot(bcsA->velocity, res.floorWallAngle);
          bcsA->velocity *= 0.8f;
        } else if(hitFloor) {
          if(bcsA->velocity.y < 0) {
            bcsA->velocity.y = 0.0f;
          } else {
            bcsA->hitTriTypes &= ~TriType::FLOOR;
          }
        }

        gameScene.onObjectCollision({bcsA, nullptr, nullptr, res.meshInstance});
      }
    }

    for(uint32_t s2=s+1; s2 < collBCS.size(); ++s2)
    {
      bool maskMatchA = bcsA->maskRead & collBCS[s2]->maskWrite;
      bool maskMatchB = collBCS[s2]->maskRead & bcsA->maskWrite;
      if(!maskMatchA && !maskMatchB)continue;

      auto bcsB = collBCS[s2];

      bool isBoxA = bcsA->flags & BCSFlags::SHAPE_BOX;
      bool isBoxB = bcsB->flags & BCSFlags::SHAPE_BOX;

      bool isColl = false;

      if(!isBoxA && !isBoxB) {
        isColl = sphereVsSphere(*bcsA, *bcsB);
      } else if(isBoxA && !isBoxB) {
        isColl = sphereVsBox(*bcsB, *bcsA);
      } else if(!isBoxA && isBoxB) {
        isColl = sphereVsBox(*bcsA, *bcsB);
      } else {
        isColl = boxVsBox(*bcsA, *bcsB);
      }

      if(isColl) {
        gameScene.onObjectCollision({bcsA, bcsB});
      }
    }

    if(bcsA->isSolid()) {
      bcsA->obj->pos = bcsA->center - bcsA->parentOffset;
    }
  }
  ticks += getTicks() - ticksStart;
}

P64::Coll::RaycastRes P64::Coll::Scene::raycast(const fm_vec3_t &pos, const fm_vec3_t &dir) {
  ++raycastCount;
  P64::Coll::RaycastRes res{};

  float highestFloor = -99999.0f;
  for(auto meshInst : meshes)
  {
    auto &mesh = *meshInst->mesh;
    auto posLocal = meshInst->intoLocalSpace(pos);
    auto dirLocal = meshInst->invRot * dir;

    P64::Coll::BVHResult bvhRes{};
    mesh.bvh->raycast(posLocal, dirLocal, bvhRes);

    for(int b=0; b<bvhRes.count; ++b) {
      uint32_t t = bvhRes.triIndex[b];

      int idxA = mesh.indices[t*3];
      int idxB = mesh.indices[t*3+1];
      int idxC = mesh.indices[t*3+2];
      auto &norm = mesh.normals[t];

      Triangle tri{
        .normal = {
         (float)norm.v[0] * (1.0f/32767.0f),
         (float)norm.v[1] * (1.0f/32767.0f),
         (float)norm.v[2] * (1.0f/32767.0f)
        },
        .v = {&mesh.verts[idxA], &mesh.verts[idxB], &mesh.verts[idxC]}
      };

      auto collInfo = mesh.vsRay(posLocal, dirLocal, tri);
      if(collInfo.hasResult())
      {
        res.flags |= collInfo.flags;
        res.hitPos = meshInst->outOfLocalSpace(collInfo.hitPos);
        {
          res.normal = meshInst->object->rot * collInfo.normal;
          highestFloor = res.hitPos.y;
        }
      }
    }
  }

  for(auto sphere : collBCS) {
    if(sphere->flags & BCSFlags::SHAPE_BOX) {
      if(pos.x >= sphere->getMinAABB().x && pos.x <= sphere->getMaxAABB().x &&
         pos.z >= sphere->getMinAABB().z && pos.z <= sphere->getMaxAABB().z)
      {
        float localHeight = sphere->center.y + sphere->halfExtend.y;
        if(localHeight > highestFloor && pos.y > localHeight) {
          highestFloor = localHeight;
          res.hitPos = {pos.x, localHeight, pos.z};
          res.normal = {0.0f, 1.0f, 0.0f};
        }
      }
    }
  }

  return res;
}

void P64::Coll::Scene::debugDraw(bool /*showMesh*/, bool /*showSpheres*/)
{
  // TODO: implement debug rendering for desktop runtime
}
