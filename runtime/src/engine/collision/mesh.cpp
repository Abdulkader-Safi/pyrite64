/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#include "engine/collision/mesh.h"

using namespace P64;

namespace {

  constexpr fm_vec3_t unitAxis0{1, 0, 0};
  constexpr fm_vec3_t unitAxis1{0, 1, 0};
  constexpr fm_vec3_t unitAxis2{0, 0, 1};

  float clamp(float x, float min, float max) {
    return fminf(fmaxf(x, min), max);
  }

  constexpr float MIN_PENETRATION = 0.0001f;

  bool intersectRaySphere(
    const fm_vec3_t &rayStarting, const fm_vec3_t &rayNormalizedDirection,
    const fm_vec3_t &sphereCenter,
    float sphereRadiusSquared,
    float &intersectionDistance
  ) {
    fm_vec3_t diff = sphereCenter - rayStarting;
    float t0 = glm::dot(diff, rayNormalizedDirection);
    float dSquared = glm::dot(diff, diff) - t0 * t0;
    if(dSquared > sphereRadiusSquared) {
      return false;
    }

    float t1 = sqrtf( sphereRadiusSquared - dSquared );
    intersectionDistance = t0 > t1 + MIN_PENETRATION ? t0 - t1 : t0 + t1;
    return intersectionDistance > MIN_PENETRATION;
  }

  [[maybe_unused]] bool intersectRaySphere(
      const fm_vec3_t &rayStarting,
      const fm_vec3_t &rayNormalizedDirection,
      const fm_vec3_t &sphereCenter,
      float sphereRadius,
      fm_vec3_t &intersectionPosition,
      fm_vec3_t &intersectionNormal
    )
  {
    float distance;
    if(intersectRaySphere(rayStarting, rayNormalizedDirection, sphereCenter, sphereRadius * sphereRadius, distance))
    {
      intersectionPosition = rayStarting + rayNormalizedDirection * distance;
      intersectionNormal = (intersectionPosition - sphereCenter) / sphereRadius;
      return true;
    }
    return false;
  }

  float pointPlaneDistance(const fm_vec3_t &p, const fm_vec3_t planePos, const fm_vec3_t planeNorm)
  {
    auto diff = (p - planePos);
    return glm::dot(diff, planeNorm);
  }

  fm_vec3_t getTriBaryCoord(const fm_vec3_t &p, const fm_vec3_t &a, const fm_vec3_t &b, const fm_vec3_t &c)
  {
    const auto v0 = c - a;
    const auto v1 = b - a;
    const auto v2 = p - a;

    const auto dot00 = glm::dot(v0, v0);
    const auto dot01 = glm::dot(v0, v1);
    const auto dot11 = glm::dot(v1, v1);

    const float denom = ( dot00 * dot11 - dot01 * dot01 );

    if(denom == 0.0f) {
      return fm_vec3_t{-1.0f, -1.0f, -1.0f};
    }

    const auto dot02 = glm::dot(v0, v2);
    const auto dot12 = glm::dot(v1, v2);

    const float invDenom = 1.0f / denom;
    const float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    const float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    return {1.0f - u-v, v, u};
  }

  fm_vec3_t closestPointOnLine(const fm_vec3_t &p, const fm_vec3_t &a, const fm_vec3_t &b)
  {
    const fm_vec3_t lineVec = b - a;
    const float length = glm::length(lineVec);
    if(length < MIN_PENETRATION)return a;
    const fm_vec3_t pointToA = p - a;
    const fm_vec3_t lineDir = lineVec / length;

    const float pointDist = glm::dot(pointToA, lineDir);
    return a + (lineDir * clamp(pointDist, 0.0f, length));
  }

  P64::Coll::CollInfo triVsSphere(const P64::Coll::BCS &sphere, const P64::Coll::Triangle &face)
  {
    const auto &bcsPos = sphere.center;

    const auto &vert0 = *face.v[0];
    const auto &vert1 = *face.v[1];
    const auto &vert2 = *face.v[2];

    float planeDist = pointPlaneDistance(bcsPos, vert0, face.normal);
    float planeDistAbs = planeDist < 0.0f ? fabsf(planeDist*2.0f) : planeDist;
    if(planeDistAbs < sphere.getRadius())
    {
      auto baryPos = getTriBaryCoord(bcsPos, vert0, vert1, vert2);
      const bool isInTri = (baryPos.x >= 0.0f) && (baryPos.y >= 0.0f)
        && ((baryPos.x + baryPos.y) <= 1.0f);

      if(isInTri)
      {
        return {
          .penetration = face.normal * (planeDist - sphere.getRadius()),
          .floorWallAngle = face.normal,
          .collCount = 1
        };
      }
    }

    const auto closestPoint1 = closestPointOnLine(bcsPos, vert0, vert1);
    const auto closestPoint2 = closestPointOnLine(bcsPos, vert1, vert2);
    const auto closestPoint3 = closestPointOnLine(bcsPos, vert2, vert0);

    const auto closestDist1 = glm::distance2(bcsPos, closestPoint1);
    const auto closestDist2 = glm::distance2(bcsPos, closestPoint2);
    const auto closestDist3 = glm::distance2(bcsPos, closestPoint3);

    const float closestDist = fminf(closestDist1, fminf(closestDist2, closestDist3));
    if(closestDist <= (sphere.getRadius() * sphere.getRadius()))
    {
      const auto contactPoint = (closestDist == closestDist1)
        ? closestPoint1
        : ((closestDist == closestDist2) ? closestPoint2 : closestPoint3);

      const auto penVector = contactPoint - bcsPos;

      const float faceDirAngle = glm::dot(penVector, face.normal);
      if(faceDirAngle > 0.0f) {
        return {.collCount = 0};
      }

      const auto penLen = glm::length(penVector);
      auto penVectorNorm = penVector / penLen * fmaxf(sphere.getRadius() - penLen, 0.0f);

      return {
        .penetration = penVectorNorm,
        .floorWallAngle = face.normal,
        .collCount = 1
      };
    }

    return {.collCount = 0};
  }

  P64::Coll::CollInfo triVsBox(const P64::Coll::BCS &box, const P64::Coll::Triangle &face)
  {
    const auto v0 = *face.v[0] - box.center;
    const auto v1 = *face.v[1] - box.center;
    const auto v2 = *face.v[2] - box.center;

    const auto edge0 = v1 - v0;
    const auto edge1 = v2 - v1;
    const auto edge2 = v0 - v2;

    float distance = 999999.0f;
    fm_vec3_t lastAxis{};

    const auto testAxis = [&v0, &v1, &v2, &box, &lastAxis, &distance](const fm_vec3_t &satAxis)
    {
      float sum = (satAxis.x + satAxis.y + satAxis.z);
      if(fabsf(sum) < 0.0001f) {
        return true;
      }

      fm_vec3_t points{
        glm::dot(v0, satAxis),
        glm::dot(v1, satAxis),
        glm::dot(v2, satAxis),
      };
      const auto combiExtend = box.halfExtend * Math::abs(satAxis);
      const float r = combiExtend.x + combiExtend.y + combiExtend.z;

      float pMin = Math::min(points);
      float pMax = Math::max(points);

      float overlap = r - fmaxf(-pMax, pMin);
      if(overlap > 0.0f) {
        if(overlap < distance) {
          distance = overlap;
          lastAxis = satAxis * Math::sign(points);
        }
        return true;
      }
      return false;
    };

    bool isColl =
      testAxis(unitAxis0) &&
      testAxis(unitAxis1) &&
      testAxis(unitAxis2) &&
      testAxis(face.normal) &&
      testAxis(Math::cross(unitAxis0, edge0)) &&
      testAxis(Math::cross(unitAxis0, edge1)) &&
      testAxis(Math::cross(unitAxis0, edge2)) &&
      testAxis(Math::cross(unitAxis1, edge0)) &&
      testAxis(Math::cross(unitAxis1, edge1)) &&
      testAxis(Math::cross(unitAxis2, edge2)) &&
      testAxis(Math::cross(unitAxis2, edge0)) &&
      testAxis(Math::cross(unitAxis2, edge1)) &&
      testAxis(Math::cross(unitAxis2, edge2))
    ;

    if(isColl) {
      return {
        .penetration = lastAxis * distance,
        .floorWallAngle = face.normal,
        .collCount = 1,
      };
    }

    return {};
  }

  bool pointVsTriangle2D(const fm_vec2_t &p, const P64::Coll::Triangle2D &tri)
  {
    bool b0 = glm::dot(fm_vec2_t{p.x - tri.v[0].x, p.y - tri.v[0].y}, fm_vec2_t{tri.v[0].y - tri.v[1].y, tri.v[1].x - tri.v[0].x}) > 0.0f;
    bool b1 = glm::dot(fm_vec2_t{p.x - tri.v[1].x, p.y - tri.v[1].y}, fm_vec2_t{tri.v[1].y - tri.v[2].y, tri.v[2].x - tri.v[1].x}) > 0.0f;
    bool b2 = glm::dot(fm_vec2_t{p.x - tri.v[2].x, p.y - tri.v[2].y}, fm_vec2_t{tri.v[2].y - tri.v[0].y, tri.v[0].x - tri.v[2].x}) > 0.0f;
    return (b0 == b1 && b1 == b2);
  }

  fm_vec3_t getTrianglePosFromXZ(const fm_vec3_t &pos, const fm_vec3_t &vert, const fm_vec3_t &normal)
  {
    const float t = (glm::dot(normal, pos) - glm::dot(normal, vert)) / normal.y;
    return pos + fm_vec3_t{0, -t, 0};
  }

  Coll::RaycastRes triVsRay3D(
    const fm_vec3_t &rayStart,
    const fm_vec3_t &dir,
    const fm_vec3_t &vert0,
    const fm_vec3_t &vert1,
    const fm_vec3_t &vert2,
    const fm_vec3_t &normal
  ) {
    float planeDist = pointPlaneDistance(rayStart, vert0, normal);
    if((planeDist < MIN_PENETRATION) || (glm::dot(dir, normal) >= 0.0f)) {
      return {};
    }

    float t = planeDist / glm::dot(dir, normal);
    fm_vec3_t hitPos = rayStart - dir * t;

    auto baryPos = getTriBaryCoord(hitPos, vert0, vert1, vert2);
    const bool isInTri = (baryPos.x >= 0.0f) && (baryPos.y >= 0.0f)
      && ((baryPos.x + baryPos.y) <= 1.0f);

    if(isInTri) {
      return {
        .hitPos = hitPos,
        .normal = normal,
        .flags = 1
      };
    }

    return {};
  }
}

P64::Coll::CollInfo P64::Coll::Mesh::vsSphere(const P64::Coll::BCS &sphere, const P64::Coll::Triangle &triangle) const {
  return triVsSphere(sphere, triangle);
}

P64::Coll::CollInfo P64::Coll::Mesh::vsBox(const P64::Coll::BCS &box, const P64::Coll::Triangle &triangle) const {
  return triVsBox(box, triangle);
}

Coll::RaycastRes Coll::Mesh::vsRay(const fm_vec3_t &rayStart, const fm_vec3_t &dir, const P64::Coll::Triangle &face) const
{
  const auto &vert0 = *face.v[0];
  const auto &vert1 = *face.v[1];
  const auto &vert2 = *face.v[2];

  {
    return triVsRay3D(rayStart, dir, vert0, vert1, vert2, face.normal);
  }

  auto tri2D = Triangle2D{{
    {vert0.x, vert0.z},
    {vert1.x, vert1.z},
    {vert2.x, vert2.z}
  }};

  if(!pointVsTriangle2D({rayStart.x, rayStart.z}, tri2D)) {
    return {};
  }

  auto hitPos = getTrianglePosFromXZ(rayStart, vert0, face.normal);
  if(hitPos.y > rayStart.y) {
    return {};
  }

  return {
    .hitPos = hitPos,
    .normal = face.normal,
    .flags = 1
  };
}
