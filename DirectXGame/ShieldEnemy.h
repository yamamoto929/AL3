#pragma once
#include "AABB.h"
#include "BaseEnemy.h"
#include "KamataEngine.h"
class GameScene;
class Player;
class ShieldEnemy final :public BaseEnemy{
private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	uint32_t textureHandle_ = 0;

	static inline const float kMoveSpeed = 0.01f;

	KamataEngine::Vector3 velocity_ = {};
	 
	static inline const float kWalkMotionAngleStart = -45.0f;
	static inline const float kWalkMotionAngleEnd = -90.0f;

	static inline const float kWalkMotionTime = 1.0f;

	float walkTimer_ = 0.0f;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	bool isDead_ = false;

	enum class Behavior { kUnknown, kWalk, kDeath,kKnockback };
	Behavior behavior_ = Behavior::kWalk;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	float deathAnimTimer_ = 0.0f;
	static inline const float kDeathAnimTimerMax_ = 0.5f;

	bool isCollidionDisabled_ = false;

	GameScene* gameScene_ = nullptr;

	enum class LRDirection {
		kRight,
		kLeft,
	};

	LRDirection lrDirection_;

	float knockbackAnimTime_ = 0.0f;
	static inline const float kKnockbackAnimTimeMax = 0.5f;

	static inline const float kKnockbackMotionAngleMax = -30.0f;

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position,GameScene* gameScene);
	void Update()override;
	void Draw()override;
	KamataEngine::Vector3 GetWorldPosition();
	AABB GetAABB()override;
	void OnCollision( Player* player)override;
	bool IsDead() const { return isDead_; }

	void BehaviorWalkInitialize();
	void BehaviorWalkUpdate();

	void BehaviorDeathInitialize();
	void BehaviorDeathUpdate();

	void BehaviorKnockbackInitialize();
	void BehaviorKnockbackUpdate();

	bool IsCollidionDisabled() const override{ return isCollidionDisabled_; };
};
