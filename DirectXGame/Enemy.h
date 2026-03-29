#pragma once
#include "KamataEngine.h"
#include "AABB.h"
class Player;
class Enemy {
private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	uint32_t textureHandle_ = 0;

	static inline const float kMoveSpeed = 0.01f;

	KamataEngine::Vector3 velocity_ = {};

	static inline const float kWalkMotionAngleStart = 0.0f;
	static inline const float kWalkMotionAngleEnd = 45.0f;

	static inline const float kWalkMotionTime = 1.0f;

	float walkTimer_ = 0.0f;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();
	KamataEngine::Vector3 GetWorldPosition();
	AABB GetAABB();
	void OnCollision(const Player* player);
};
