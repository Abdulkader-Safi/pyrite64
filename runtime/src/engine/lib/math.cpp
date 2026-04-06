/**
* @copyright 2024 - Max Bebök
* @license MIT
*/
#define GLM_ENABLE_EXPERIMENTAL
#include "engine/lib/math.h"

fm_quat_t P64::Math::unpackQuat(uint32_t quatPacked)
{
  constexpr int BITS = 10;
  constexpr int BIT_MASK = (1 << BITS) - 1;

  uint32_t largestIdx = quatPacked >> 30;
  uint32_t idx0 = (largestIdx + 1) & 0b11;
  uint32_t idx1 = (largestIdx + 2) & 0b11;
  uint32_t idx2 = (largestIdx + 3) & 0b11;

  float q0 = s10ToFloat((quatPacked >> (BITS * 2)) & BIT_MASK, -SQRT_2_INV, SQRT_2_INV*2);
  float q1 = s10ToFloat((quatPacked >> (BITS * 1)) & BIT_MASK, -SQRT_2_INV, SQRT_2_INV*2);
  float q2 = s10ToFloat((quatPacked >> (BITS * 0)) & BIT_MASK, -SQRT_2_INV, SQRT_2_INV*2);

  // GLM quat layout: w,x,y,z — access via array operator [0]=x,[1]=y,[2]=z,[3]=w
  fm_quat_t q{1.0f, 0.0f, 0.0f, 0.0f}; // identity (w,x,y,z)
  q[idx0] = q0;
  q[idx1] = q1;
  q[idx2] = q2;
  float wSq = 1.0f - q0*q0 - q1*q1 - q2*q2;
  q[largestIdx] = (wSq > 0.0f) ? sqrtf(wSq) : 0.0f;

  if(q.w > 0.9999f) {
    q = fm_quat_t{1.0f, 0.0f, 0.0f, 0.0f}; // identity
  }

  return q;
}

fm_quat_t P64::Math::quatFromInvRotMat(const fm_mat4_t &m)
{
  float trace = m[0][0] + m[1][1] + m[2][2];
  fm_quat_t q;

  if (trace > 0.0f) {
    float s = sqrtf(trace + 1.0f) * 2.0f;
    q.w = 0.25f * s;
    q.x = (m[2][1] - m[1][2]) / s;
    q.y = (m[0][2] - m[2][0]) / s;
    q.z = (m[1][0] - m[0][1]) / s;
  } else if ((m[0][0] > m[1][1]) && (m[0][0] > m[2][2])) {
    float s = sqrtf(1.0f + m[0][0] - m[1][1] - m[2][2]) * 2.0f;
    q.w = (m[2][1] - m[1][2]) / s;
    q.x = 0.25f * s;
    q.y = (m[0][1] + m[1][0]) / s;
    q.z = (m[0][2] + m[2][0]) / s;
  } else if (m[1][1] > m[2][2]) {
    float s = sqrtf(1.0f + m[1][1] - m[0][0] - m[2][2]) * 2.0f;
    q.w = (m[0][2] - m[2][0]) / s;
    q.x = (m[0][1] + m[1][0]) / s;
    q.y = 0.25f * s;
    q.z = (m[1][2] + m[2][1]) / s;
  } else {
    float s = sqrtf(1.0f + m[2][2] - m[0][0] - m[1][1]) * 2.0f;
    q.w = (m[1][0] - m[0][1]) / s;
    q.x = (m[0][2] + m[2][0]) / s;
    q.y = (m[1][2] + m[2][1]) / s;
    q.z = 0.25f * s;
  }

  return q;
}
