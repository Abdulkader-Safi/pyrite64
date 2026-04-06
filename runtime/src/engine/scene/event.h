/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#pragma once
#include <cstdint>

namespace P64
{
  constexpr uint32_t MAX_EVENT_COUNT = 128;

  constexpr uint16_t EVENT_TYPE_ENABLE = 0xFFFF;
  constexpr uint16_t EVENT_TYPE_DISABLE = 0xFFFE;

  constexpr uint16_t EVENT_TYPE_CUSTOM_START = 0x0000;
  constexpr uint16_t EVENT_TYPE_CUSTOM_END   = 0xF000;

  struct ObjectEvent
  {
    uint16_t senderId{};
    uint16_t type{};
    uint32_t value{};
  };

  struct ObjectEventWrapper
  {
    ObjectEvent event{};
    uint16_t targetId{};
  };

  struct ObjectEventQueue
  {
    ObjectEventWrapper events[MAX_EVENT_COUNT]{};
    uint32_t eventCount{0};

    void add(uint16_t targetId, uint16_t senderId, uint16_t type, uint32_t value) {
      if (eventCount < MAX_EVENT_COUNT) {
        events[eventCount].targetId = targetId;
        events[eventCount].event = {
          .senderId = senderId,
          .type = type,
          .value = value
        };
        eventCount++;
      }
    }

    void clear() {
      eventCount = 0;
    }
  };
}
