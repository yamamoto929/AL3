#pragma once
#include "KamataEngine.h"

KamataEngine::Vector3& operator+=(KamataEngine::Vector3& lhs, const KamataEngine::Vector3& rhs);
KamataEngine::Vector3 operator+(const KamataEngine::Vector3& lhs, const KamataEngine::Vector3& rhs);

KamataEngine::Vector3& operator-=(KamataEngine::Vector3& lhs, const KamataEngine::Vector3& rhs);
KamataEngine::Vector3 operator-(const KamataEngine::Vector3& lhs, const KamataEngine::Vector3& rhs);

KamataEngine::Vector3& operator*=(KamataEngine::Vector3& lhs, const KamataEngine::Vector3& rhs);
KamataEngine::Vector3 operator*(const KamataEngine::Vector3& lhs, const KamataEngine::Vector3& rhs);

KamataEngine::Vector3& operator/=(KamataEngine::Vector3& lhs, const KamataEngine::Vector3& rhs);
KamataEngine::Vector3 operator/(const KamataEngine::Vector3& lhs, const KamataEngine::Vector3& rhs);
KamataEngine::Vector3 operator/(const KamataEngine::Vector3& lhs, float scalar);
