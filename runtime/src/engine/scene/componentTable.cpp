/**
* @copyright 2025 - Max Bebök
* @license MIT
*/
#include "engine/scene/componentTable.h"
#include "engine/scene/object.h"

// All component stubs — most are empty for now
#include "engine/scene/components/code.h"
#include "engine/scene/components/model.h"
#include "engine/scene/components/light.h"
#include "engine/scene/components/camera.h"
#include "engine/scene/components/collMesh.h"
#include "engine/scene/components/collBody.h"
#include "engine/scene/components/audio2d.h"
#include "engine/scene/components/constraint.h"
#include "engine/scene/components/culling.h"
#include "engine/scene/components/nodeGraph.h"
#include "engine/scene/components/animModel.h"

namespace {
  // No-op implementations for stubbed components
  void nopInitDel(P64::Object&, void*, void*) {}
  void nopUpdate(P64::Object&, void*, float) {}
  void nopDraw(P64::Object&, void*, float) {}
  uint32_t nopGetAllocSize(void*) { return 0; }
}

// Component table — maps component IDs to their lifecycle functions
// All components are stubbed with no-ops for now; they'll be fleshed out in later phases
const P64::ComponentDef P64::COMP_TABLE[P64::COMP_TABLE_SIZE] = {
  /* 0: Code      */ { nopInitDel, nopUpdate, nullptr, nullptr, nullptr, nopGetAllocSize },
  /* 1: Model     */ { nopInitDel, nullptr, nopDraw, nullptr, nullptr, nopGetAllocSize },
  /* 2: Light     */ { nopInitDel, nullptr, nopDraw, nullptr, nullptr, nopGetAllocSize },
  /* 3: Camera    */ { nopInitDel, nopUpdate, nopDraw, nullptr, nullptr, nopGetAllocSize },
  /* 4: CollMesh  */ { nopInitDel, nullptr, nullptr, nullptr, nullptr, nopGetAllocSize },
  /* 5: CollBody  */ { nopInitDel, nopUpdate, nullptr, nullptr, nullptr, nopGetAllocSize },
  /* 6: Audio2D   */ { nopInitDel, nullptr, nullptr, nullptr, nullptr, nopGetAllocSize },
  /* 7: Constraint*/ { nopInitDel, nullptr, nopDraw, nullptr, nullptr, nopGetAllocSize },
  /* 8: Culling   */ { nopInitDel, nullptr, nopDraw, nullptr, nullptr, nopGetAllocSize },
  /* 9: NodeGraph */ { nopInitDel, nopUpdate, nullptr, nullptr, nullptr, nopGetAllocSize },
  /*10: AnimModel */ { nopInitDel, nullptr, nopDraw, nullptr, nullptr, nopGetAllocSize },
  /*11: unused    */ {},
  /*12: unused    */ {},
  /*13: unused    */ {},
  /*14: unused    */ {},
  /*15: unused    */ {},
};
