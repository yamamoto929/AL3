#include "Vector3Operator.h"

using namespace KamataEngine;

Vector3& operator+=(Vector3& lhs, const Vector3& rhs) {
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
	return lhs;
}

Vector3 operator+(const Vector3& lhs, const Vector3& rhs) {
	Vector3 result = lhs;
	result += rhs;
	return result;
}

Vector3& operator-=(Vector3& lhs, const Vector3& rhs) {
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	lhs.z -= rhs.z;
	return lhs;
}

Vector3 operator-(const Vector3& lhs, const Vector3& rhs) {
	Vector3 result = lhs;
	result -= rhs;
	return result;
}

Vector3& operator*=(Vector3& lhs, const Vector3& rhs) {
	lhs.x *= rhs.x;
	lhs.y *= rhs.y;
	lhs.z *= rhs.z;
	return lhs;
}

Vector3 operator*(const Vector3& lhs, const Vector3& rhs) {
	Vector3 result = lhs;
	result *= rhs;
	return result;
}

Vector3& operator/=(Vector3& lhs, const Vector3& rhs) {
	lhs.x /= rhs.x;
	lhs.y /= rhs.y;
	lhs.z /= rhs.z;
	return lhs;
}

Vector3 operator/(const Vector3& lhs, const Vector3& rhs) {
	Vector3 result = lhs;
	result /= rhs;
	return result;
}

Vector3 operator/(const Vector3& lhs, float scalar) {
	Vector3 result = lhs;
	result.x /= scalar;
	result.y /= scalar;
	result.z /= scalar;
	return result;
};
