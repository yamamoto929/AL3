#include "GuardEffect.h"
#include <cassert>
#include "Easing.h"
#include "WorldMatrixUpdate.h"
#include "RandomNumberGenerator.h"
#include "KamataEngine.h"
#include <numbers>

using namespace KamataEngine;
void GuardEffect::Initialize(Vector3 spawnPoint, Model* model, Camera* camera) {
	model_ = model;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = spawnPoint;

	circleColor_.Initialize();
	circleColorAlpha_ = 1.0f;

	animCount_ = 0.0f;
	state_ = State::kStop;
}


void GuardEffect::Update() {
	animCount_ += 1.0f / 60.0f;
	switch (state_) {
	case State::kUnknown:
	default:
		break;
	case State::kStop: {
		// ズームアウト移行
		if (animCount_ >= kStopTime) {
			state_ = State::kZoomOut;
			animCount_ = 0.0f;
		}
		break;
	}
	case State::kZoomOut: {
		float t = animCount_ / kFadeTime;
		worldTransform_.scale_.z = EaseOut(1.0f, 1.5f, t);
		worldTransform_.scale_.y = EaseOut(1.0f, 1.5f, t);
		worldTransform_.scale_.x = EaseOut(1.0f, 1.5f, t);
		if (animCount_ >= kZoomOutTime) {
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

	WorldMatrixUpdate(worldTransform_);
}

void GuardEffect::Draw() {
	if (camera_ == nullptr || state_ == State::kDeath || model_ == nullptr) {
		return;
	}

	circleColor_.SetColor(Vector4{1.0f, 1.0f, 1.0f, circleColorAlpha_});
	model_->Draw(worldTransform_, *camera_, &circleColor_);
}

GuardEffect* GuardEffect::Create(KamataEngine::Vector3 spawnPoint, KamataEngine::Model* model, KamataEngine::Camera* camera) {
	GuardEffect* instance = new GuardEffect();
	assert(instance);
	instance->Initialize(spawnPoint, model, camera);
	return instance;
}