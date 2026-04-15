#define NOMINMAX
#include "CameraController.h"
#include <algorithm>

using namespace KamataEngine;

void CameraController::Initialize(Camera* camera) { camera_ = camera; }

Vector3 Lerp(Vector3 a, Vector3 b, float t);

void CameraController::Update() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	Vector3 targetVelocity = target_->GetVelocity();
	targetPosition_ = targetWorldTransform.translation_ + targetOffset_;

	camera_->translation_ = Lerp(camera_->translation_, targetPosition_, kInterpolationRate);

	// マージン（遊び）の適用
	camera_->translation_.x = std::max(camera_->translation_.x, targetPosition_.x + margin.left);
	camera_->translation_.x = std::min(camera_->translation_.x, targetPosition_.x + margin.right);
	camera_->translation_.y = std::max(camera_->translation_.y, targetPosition_.y + margin.bottom);
	camera_->translation_.y = std::min(camera_->translation_.y, targetPosition_.y + margin.top);

	// 可動域の制限
	camera_->translation_.x = std::clamp(camera_->translation_.x, movableArea_.left, movableArea_.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, movableArea_.bottom, movableArea_.top);

	camera_->UpdateMatrix();
}

void CameraController::Reset() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;
};

Vector3 Lerp(Vector3 a, Vector3 b, float t) {
	Vector3 result{};
	result.x = a.x + (b.x - a.x) * t;
	result.y = a.y + (b.y - a.y) * t;
	result.z = a.z + (b.z - a.z) * t;
	return result;
}