#include "HitEffect.h"
#include <cassert>
#include "Easing.h"
#include "WorldMatrixUpdate.h"
#include "RandomNumberGenerator.h"
#include "KamataEngine.h"
#include <numbers>

using namespace KamataEngine;
void HitEffect::Initialize(Vector3 spawnPoint, Model* model, Camera* camera) {
	model_ = model;
	camera_ = camera;

	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_ = spawnPoint;

	circleColor_.Initialize();
	circleColorAlpha_ = 1.0f;

	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		worldTransform.scale_ = {0.1f, 1.6f, 1.0f};
		float rand = RNG::GetInstance().Generate(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);
		worldTransform.rotation_ = {0.0f, 0.0f, rand};
		worldTransform.translation_ = spawnPoint;

		worldTransform.Initialize();
	}
	animCount_ = 0.0f;
	state_ = State::kSpread;
}


void HitEffect::Update() {
	animCount_ += 1.0f / 60.0f;
	switch (state_) {
	case State::kUnknown:
	default:
		break;
	case State::kSpread: {
		float t = animCount_ / kSpreadTime;
		circleWorldTransform_.scale_.z = EaseOut(0.5f, 1.2f, t);
		circleWorldTransform_.scale_.y = EaseOut(0.5f, 1.2f, t);
		circleWorldTransform_.scale_.x = EaseOut(0.5f, 1.2f, t);

		// フェードアウト移行
		if (animCount_ >= kSpreadTime) {
			state_ = State::kFade;
			animCount_ = 0.0f;
		}
		break;
	}
	case State::kFade: {
		float t = animCount_ / kFadeTime;
		circleColorAlpha_ = EaseOut(1.0f, 0.2f, t);

		// デス移行
		if (animCount_ >= kFadeTime) {
			state_ = State::kDeath;
			animCount_ = 0.0f;
		}
		break;
	}
	case State::kDeath:
		break;
	}

	WorldMatrixUpdate(circleWorldTransform_);
	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		WorldMatrixUpdate(worldTransform);
	}
}

void HitEffect::Draw() {
	if (camera_ == nullptr || state_ == State::kDeath || model_ == nullptr) {
		return;
	}

	circleColor_.SetColor(Vector4{1.0f, 1.0f, 1.0f, circleColorAlpha_});
	model_->Draw(circleWorldTransform_, *camera_, &circleColor_);

	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		model_->Draw(worldTransform, *camera_, &circleColor_);
	}
}

HitEffect* HitEffect::Create(KamataEngine::Vector3 spawnPoint, KamataEngine::Model* model, KamataEngine::Camera* camera) {
	HitEffect* instance = new HitEffect();
	assert(instance);
	instance->Initialize(spawnPoint, model, camera);
	return instance;
}