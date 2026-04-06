/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "engine/scene/scene.h"
#include "engine/scene/globalState.h"
#include "engine/scene/componentTable.h"
#include "engine/scene/sceneManager.h"
#include "engine/lib/logger.h"

#include <chrono>
#include <cstring>

P64::GlobalState P64::state{};

namespace {
  uint64_t getTicks() {
    return std::chrono::steady_clock::now().time_since_epoch().count();
  }
}

P64::Scene::Scene(uint16_t sceneId, Scene** ref)
  : id{sceneId}
{
  *ref = this;
  idLookup.fill(nullptr);
  // TODO: load scene data from exported assets
}

P64::Scene::~Scene()
{
  for(auto obj : objects) {
    obj->~Object();
    free(obj);
  }
  objects.clear();
}

void P64::Scene::update(float deltaTime)
{
  auto ticksStart = getTicks();

  // Process events from previous frame
  auto &readQueue = eventQueue[1 - eventQueueIdx];
  for(uint32_t i=0; i < readQueue.eventCount; ++i) {
    auto &ev = readQueue.events[i];
    auto obj = getObjectById(ev.targetId);
    if(!obj || !obj->isEnabled())continue;

    auto compRefs = obj->getCompRefs();
    for(uint32_t c=0; c<obj->compCount; ++c) {
      const auto &compDef = COMP_TABLE[compRefs[c].type];
      if(compDef.onEvent) {
        char* dataPtr = (char*)obj + compRefs[c].offset;
        compDef.onEvent(*obj, dataPtr, ev.event);
      }
    }
  }
  readQueue.clear();
  eventQueueIdx = 1 - eventQueueIdx;

  // Update components
  for(auto obj : objects) {
    if(!obj->isEnabled())continue;

    auto compRefs = obj->getCompRefs();
    for(uint32_t c=0; c<obj->compCount; ++c) {
      const auto &compDef = COMP_TABLE[compRefs[c].type];
      if(compDef.update) {
        char* dataPtr = (char*)obj + compRefs[c].offset;
        compDef.update(*obj, dataPtr, deltaTime);
      }
    }
  }

  // Collision
  collScene.update(deltaTime);

  // Pending add/remove
  for(auto obj : pendingObjDelete) {
    std::erase(objects, obj);
    if(obj->id < idLookup.size()) {
      idLookup[obj->id] = nullptr;
    }
    obj->~Object();
    free(obj);
  }
  pendingObjDelete.clear();

  ticksActorUpdate += getTicks() - ticksStart;
}

void P64::Scene::draw(float deltaTime)
{
  auto ticksStart = getTicks();

  lighting.reset();

  for(auto obj : objects) {
    if(!obj->isEnabled())continue;
    if(obj->flags & ObjectFlags::IS_CULLED)continue;

    auto compRefs = obj->getCompRefs();
    for(uint32_t c=0; c<obj->compCount; ++c) {
      const auto &compDef = COMP_TABLE[compRefs[c].type];
      if(compDef.draw) {
        char* dataPtr = (char*)obj + compRefs[c].offset;
        compDef.draw(*obj, dataPtr, deltaTime);
      }
    }
  }

  lighting.apply();

  ticksDraw += getTicks() - ticksStart;
}

void P64::Scene::onObjectCollision(const Coll::CollEvent &event)
{
  auto handleObj = [&](Coll::BCS *bcs, Coll::MeshInstance *mesh) {
    if(!bcs && !mesh)return;
    Object *obj = bcs ? bcs->obj : mesh->object;
    if(!obj || !obj->isEnabled())return;

    auto compRefs = obj->getCompRefs();
    for(uint32_t c=0; c<obj->compCount; ++c) {
      const auto &compDef = COMP_TABLE[compRefs[c].type];
      if(compDef.onColl) {
        char* dataPtr = (char*)obj + compRefs[c].offset;
        compDef.onColl(*obj, dataPtr, event);
      }
    }
  };

  handleObj(event.selfBCS, event.selfMesh);
  handleObj(event.otherBCS, event.otherMesh);
}

uint16_t P64::Scene::addObject(
  uint32_t /*prefabIdx*/,
  const fm_vec3_t &/*pos*/,
  const fm_vec3_t &/*scale*/,
  const fm_quat_t &/*rot*/)
{
  // TODO: implement prefab spawning for runtime
  return 0;
}

void P64::Scene::removeObject(Object &obj)
{
  pendingObjDelete.push_back(&obj);
}

P64::Object* P64::Scene::getObjectById(uint16_t objId) const
{
  if(objId < idLookup.size()) {
    return idLookup[objId];
  }
  for(auto obj : objects) {
    if(obj->id == objId)return obj;
  }
  return nullptr;
}

void P64::Scene::setGroupEnabled(uint16_t groupId, bool enabled) const
{
  for(auto obj : objects) {
    if(obj->group == groupId) {
      obj->setFlag(ObjectFlags::PARENTS_ACTIVE, enabled);
    }
  }
}

P64::Lighting& P64::Scene::startLightingOverride(bool copyExisting)
{
  if(copyExisting) {
    lightingTemp = lighting;
  } else {
    lightingTemp = {};
  }
  return lightingTemp;
}

void P64::Scene::endLightingOverride()
{
  lighting = lightingTemp;
}
