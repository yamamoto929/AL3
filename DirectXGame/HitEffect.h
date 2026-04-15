#pragma once
#include "BaseEffect.h"

class HitEffect final : public BaseEffect {
private:
	enum class State { kUnknown, kSpread, kFade, kDeath };
	static inline const size_t kEllipseNum = 2;
	static inline const float kSpreadTime = 1.0f;
	static inline const float kFadeTime = 0.5f;

	State state_ = State::kUnknown;

	float circleColorAlpha_ = 0.0f;
	KamataEngine::ObjectColor circleColor_;

	KamataEngine::WorldTransform circleWorldTransform_;
	std::array<KamataEngine::WorldTransform, kEllipseNum> ellipseWorldTransforms_;

public:
	void Initialize(KamataEngine::Vector3 spawnPoint, KamataEngine::Model* model, KamataEngine::Camera* camera);
	void Update()override;
	void Draw()override;
	static HitEffect* Create(KamataEngine::Vector3 spawnPoint, KamataEngine::Model* model, KamataEngine::Camera* camera);
	bool IsDead() const override{ return state_ == State::kDeath; }
};