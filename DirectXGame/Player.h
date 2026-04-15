#pragma once
#include "AABB.h"
#include "BaseEnemy.h"
#include "KamataEngine.h"
#include "MapChipField.h"

class MapChipField;
class Enemy;
class Player {
public:
	struct CollisionMapInfo {
		bool isCollideCeiling = false;
		bool isLanding = false;
		bool isTouchingWall = false;
		KamataEngine::Vector3 velocity = KamataEngine::Vector3{0.0f, 0.0f, 0.0f};
	};

	enum Corner { kRIGHTBOTTOM, kLEFTBOTTOM, kRIGHTTOP, kLEFTTOP, kNUMCORNER };

	enum class Behavior { kUnknown, kRoot, kAttack,kKnockback };

	enum class AttackPhase {
		kPrepare, // ため時間
		kRush,    // 突撃
		kRecovery // 余韻
	};

	enum class KnockbackPhase {
		kLaunched,
		kRecovery
	};

	enum class LRDirection {
		kRight,
		kLeft,
	};

private:
	uint32_t textureHandle_ = 0;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelAttack_ = nullptr;

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::WorldTransform worldTransformAttack_;

	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::Vector3 position_ = {};
	KamataEngine::Vector3 velocity_ = {};

	static inline const float kAcceleration = 0.1f;
	static inline const float kAttenuation = 0.1f;
	static inline const float kLimitRunSpeed = 0.5f;

	

	LRDirection lrDirection_ = LRDirection::kRight;

	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	static inline const float kTimeTurn = 0.3f;

	bool onGround_ = true;

	MapChipField* mapChipField_ = nullptr;

	// 重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.02f;
	// 最大落下速度(下方向)
	static inline const float kLimitFallSpeed = -0.4f;
	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 0.5f;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 0.01f;

	static inline const float kAttenuationLanding = 0.5f;
	static inline const float kAttenuationWall = 0.5f;
	static inline const float kGroundSnapOffset = 0.011f;

	// デスフラグ
	bool isDead_ = false;

	float movingAttackCount_ = 0.0f;
	static inline const float kMovingAttackCountMax_ = 1.0f;

	Behavior behavior_ = Behavior::kRoot;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	AttackPhase attackPhase_;
	static inline const float kPrepareTime = 0.05f;
	static inline const float kRushTime = 0.3f;
	static inline const float kAttackRecoveryTime = 0.05f;

	static inline const float kAttackVelocity = 0.3f;

	bool canRush_ = true;
	bool isAttack_ = false;

	KnockbackPhase knockbackPhase_;
	float knockbackCount_ = 0.0f;
	static inline const float kKnockbackRecoveryTime_ = 0.1f;
	static inline const float kKnockbackLaunchedTime_ = 0.3f;

	static inline const float kKnockbackVelocity = 0.3f;
	
	bool isKnockbackRequested_ = false;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="camera">カメラ</param>
	/// <param name="position">位置</param>
	void Initialize(KamataEngine::Model* model, KamataEngine::Model* modelAttack, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	KamataEngine::Vector3 GetWorldPosition();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void Move();

	void CheckMapCollidion(CollisionMapInfo& info);

	void CheckMapCollidionTop(CollisionMapInfo& info);
	void CheckMapCollidionBottom(CollisionMapInfo& info);
	void CheckMapCollidionLeft(CollisionMapInfo& info);
	void CheckMapCollidionRight(CollisionMapInfo& info);

	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	void MoveByResult(const CollisionMapInfo& info);

	void OnContactCeiling(const CollisionMapInfo& info);

	void GroundedStatusHandling(const CollisionMapInfo& info);

	void OnContactWall(const CollisionMapInfo& info);

	AABB GetAABB();

	//void OnCollision(const Enemy* enemy);

	void OnCollision(const BaseEnemy* enemy);

	bool IsDead() const { return isDead_; }

	void BehaviorRootUpdate();
	void BehaviorRootInitialize();
	void BehaviorAttackUpdate();
	void BehaviorAttackInitialize();
	void BehaviorKnockbackInitialize();
	void BehaviorKnockbackUpdate();

	bool IsAttack() const { return isAttack_; }
	LRDirection GetLRDirection() const { return lrDirection_; }

	void KnockbackRequest();
};