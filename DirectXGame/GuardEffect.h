#pragma once
#include "BaseEffect.h"

class GuardEffect final : public BaseEffect {
private:
	enum class State { kUnknown, kStop,kZoomOut, kFade, kDeath };

	static inline const float kStopTime = 0.1f;
	static inline const float kZoomOutTime = 0.2f;
	static inline const float kFadeTime = 0.1f;

	float circleColorAlpha_ = 0.0f;
	KamataEngine::ObjectColor circleColor_;
	KamataEngine::WorldTransform worldTransform_;

	State state_ = State::kUnknown;

public:
	void Initialize(KamataEngine::Vector3 spawnPoint, KamataEngine::Model* model, KamataEngine::Camera* camera);
	void Update() override;
	void Draw() override;
	static GuardEffect* Create(KamataEngine::Vector3 spawnPoint, KamataEngine::Model* model, KamataEngine::Camera* camera);
	bool IsDead() const override{ return state_ == State::kDeath; }
};