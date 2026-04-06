/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "engine/collision/mesh.h"
#include "engine/collision/bvh.h"

namespace {
  char* align(char* ptr, size_t alignment) {
    return (char*)(((uintptr_t)ptr + alignment - 1) & ~(alignment - 1));
  }
}

P64::Coll::Mesh* P64::Coll::Mesh::load(void* rawData)
{
  Mesh* mesh = (Mesh*)rawData;

  char* data = (char*)&mesh->indices[0];

  data += mesh->triCount * sizeof(int16_t) * 3;
  data = align(data, 4);
  mesh->normals = (IVec3*)data;

  data += mesh->triCount * sizeof(IVec3);
  data = align(data, 4);
  mesh->verts = (fm_vec3_t *)data;

  data += mesh->vertCount * sizeof(fm_vec3_t);
  data = align(data, 4);
  mesh->bvh = (BVH*)data;

  return mesh;
}
