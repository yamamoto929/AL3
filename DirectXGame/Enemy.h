#pragma once
#include "AABB.h"
#include "KamataEngine.h"
class GameScene;
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

	bool isDead_ = false;

	enum class Behavior { kUnknown, kWalk, kDeath };
	Behavior behavior_ = Behavior::kWalk;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	float deathAnimTimer_ = 0.0f;
	static inline const float kDeathAnimTimerMax_ = 0.5f;

	bool isCollidionDisabled_ = false;

	GameScene* gameScene_ = nullptr;

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position,GameScene* gameScene);
	void Update();
	void Draw();
	KamataEngine::Vector3 GetWorldPosition();
	AABB GetAABB();
	void OnCollision(const Player* player);
	bool IsDead() const { return isDead_; }

	void BehaviorWalkInitialize();
	void BehaviorWalkUpdate();

	void BehaviorDeathInitialize();
	void BehaviorDeathUpdate();

	bool IsCollidionDisabled() const{ return isCollidionDisabled_; };
};
