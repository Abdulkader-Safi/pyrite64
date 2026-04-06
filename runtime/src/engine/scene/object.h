/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <cstdint>
#include "engine/lib/math.h"
#include "objectFlags.h"
#include "event.h"
#include "sceneManager.h"

namespace P64
{
  class Scene;

  class Object
  {
    public:
      struct CompRef
      {
        uint8_t type{};
        uint8_t flags{};
        uint16_t offset{};
      };

      ~Object();

      uint16_t id{};
      uint16_t group{};
      uint16_t flags{};
      uint16_t compCount{0};

      fm_quat_t rot{1.0f, 0.0f, 0.0f, 0.0f};
      fm_vec3_t pos{};
      fm_vec3_t scale{1.0f};

      void setFlag(uint16_t flag, bool enabled) {
        if(enabled) {
          this->flags |= flag;
        } else {
          this->flags &= ~flag;
        }
      }

      [[nodiscard]] CompRef* getCompRefs() const {
        return (CompRef*)((uint8_t*)this + sizeof(Object));
      }

      [[nodiscard]] char* getCompData() const {
        return (char*)getCompRefs() + sizeof(CompRef) * compCount;
      }

      template<typename T>
      [[nodiscard]] T* getComponent() const {
        auto compRefs = getCompRefs();
        for (uint32_t i=0; i<compCount; ++i) {
          if(compRefs[i].type == T::ID) {
            return (T*)((char*)this + compRefs[i].offset);
          }
        }
        return nullptr;
      }

      template<typename T>
      [[nodiscard]] T* getComponent(uint32_t idx) const {
        auto compRefs = getCompRefs();
        for (uint32_t i=0; i<compCount; ++i) {
          if(compRefs[i].type == T::ID) {
            if (idx-- == 0) {
              return (T*)((char*)this + compRefs[i].offset);
            }
          }
        }
        return nullptr;
      }

      [[nodiscard]] bool isSelfEnabled() const {
        return (flags & ObjectFlags::SELF_ACTIVE);
      }

      [[nodiscard]] bool isEnabled() const {
        return (flags & ObjectFlags::ACTIVE) == ObjectFlags::ACTIVE;
      }

      void setEnabled(bool isEnabled);

      [[nodiscard]] bool hasChildren() const {
        return (flags & ObjectFlags::HAS_CHILDREN);
      }

      void remove();

      static Scene& getScene() {
        return SceneManager::getCurrent();
      }

      template<typename F, typename SCENE = Scene>
      void iterChildren(F&& f) {
        const SCENE &sc = getScene();
        sc.iterObjectChildren(id, f);
      }

      template<typename SCENE = Scene>
      Object* getParent() {
        const SCENE &sc = getScene();
        return sc.getObjectById(this->group);
      }

      [[nodiscard]] fm_vec3_t intoLocalSpace(const fm_vec3_t &p) const;
      [[nodiscard]] fm_vec3_t outOfLocalSpace(const fm_vec3_t &p) const;
  };

  struct ObjectRef
  {
    uint32_t id{};

    [[nodiscard]] Object* get() const;

    [[nodiscard]] Object* operator->() const {
      return get();
    }

    [[nodiscard]] operator Object*() const {
      return get();
    }

    [[nodiscard]] explicit operator bool() const {
      return get() != nullptr;
    }
  };
}
