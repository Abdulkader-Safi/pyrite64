/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <vector>
#include <array>
#include <functional>

#include "event.h"
#include "lighting.h"
#include "object.h"
#include "engine/collision/scene.h"
#include "engine/lib/types.h"

namespace P64
{
  struct SceneConf {
    uint16_t screenWidth{};
    uint16_t screenHeight{};
    uint32_t flags{};
    uint8_t clearColor[4]{};
    uint32_t objectCount{};
    uint8_t frameSkip{};
    uint8_t filter{};
    uint16_t audioFreq{};
  };

  struct PrefabParams
  {
    void* prefabData{nullptr};
    fm_vec3_t pos{0,0,0};
    fm_vec3_t scale{1,1,1};
    fm_quat_t rot{1,0,0,0};
    uint16_t objectId{0};
  };

  class Scene
  {
    private:
      std::vector<Object*> objects{};
      std::vector<PrefabParams> objectsToAdd{};

      struct PendingCompInit
      {
        Object* obj{};
        char* dataPtr{};
        uint8_t compId{};
        uint8_t* initData{};
      };
      std::vector<PendingCompInit> pendingCompInit{};

      std::array<Object*, 128> idLookup{};

      Coll::Scene collScene{};
      std::vector<Object*> pendingObjDelete{};

      uint32_t eventQueueIdx{0};
      ObjectEventQueue eventQueue[2]{};

      Lighting lighting{};
      Lighting lightingTemp{};

      SceneConf conf{};
      uint16_t id{};

      Object* loadObject(uint8_t* &objFile, std::function<void(Object&)> callback = {}, bool deferComponentInit = false);
      void runPendingComponentInit();

    public:
      uint64_t ticksActorUpdate{0};
      uint64_t ticksGlobalUpdate{0};
      uint64_t ticksGlobalDraw{0};
      uint64_t ticksDraw{0};

      explicit Scene(uint16_t sceneId, Scene** ref);
      ~Scene();

      CLASS_NO_COPY_MOVE(Scene);

      void update(float deltaTime);
      void draw(float deltaTime);

      [[nodiscard]] SceneConf& getConf() { return conf; }
      [[nodiscard]] uint16_t getId() const { return id; }
      Coll::Scene &getCollision() { return collScene; }

      void onObjectCollision(const Coll::CollEvent &event);

      void sendEvent(uint16_t targetId, uint16_t senderId, uint16_t type, uint32_t value) {
        eventQueue[eventQueueIdx].add(targetId, senderId, type, value);
      }

      uint16_t addObject(
        uint32_t prefabIdx,
        const fm_vec3_t &pos = {0,0,0},
        const fm_vec3_t &scale = {1,1,1},
        const fm_quat_t &rot = {1,0,0,0}
      );

      void removeObject(Object &obj);

      Object* getObjectById(uint16_t objId) const;

      uint32_t getObjectCount() const { return objects.size(); }

      template<typename F>
      void iterObjectChildren(uint16_t parentId, F&& f) const {
        for (auto o : objects) {
          if(o->group != parentId)continue;
          f(o);
        }
      }

      void setGroupEnabled(uint16_t groupId, bool enabled) const;

      [[nodiscard]] Lighting& getLighting() { return lighting; }

      [[nodiscard]] Lighting& startLightingOverride(bool copyExisting = true);
      void endLightingOverride();
  };
}

#include "object.h"
