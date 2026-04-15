#pragma once
#include "KamataEngine.h"
#include "Matrix4x4Calc.h"
#include "Vector3Operator.h"
#include "WorldMatrixUpdate.h"
#include <array>
#include <algorithm>
#include <numbers>

class DeathParticles {
private:
	KamataEngine::Camera* camera_;
	KamataEngine::Model* model_;
	static inline const uint32_t kNumParticles = 8;
	static inline const float kDuration = 2.0f;
	static inline const float kSpeed = 0.05f;
	static inline const float kAngleUnit = std::numbers::pi_v<float> * 2.0f / static_cast<float>(kNumParticles);

	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransforms_;

	bool finished_ = false;
	float counter_ = 0.0f;

	KamataEngine::ObjectColor objectColor_;
	KamataEngine::Vector4 color_;

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();
	bool IsFinished() { return finished_; }
};
