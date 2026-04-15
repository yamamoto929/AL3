#include "DeathParticles.h"

using namespace KamataEngine;

void DeathParticles::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	model_ = model;
	camera_ = camera;
	for (WorldTransform& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}
	objectColor_.Initialize();
	color_ = {1.0f, 1.0f, 1.0f, 1.0f};
};

void DeathParticles::Update() {
	if (finished_) {
		return;
	}

	for (size_t i = 0; i < kNumParticles; ++i) {
		Vector3 velocity = Vector3{kSpeed, 0.0f, 0.0f};
		float angle = kAngleUnit * i;
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);
		velocity = Transform(velocity, matrixRotation);
		worldTransforms_[i].translation_ += velocity;
	}

	counter_ += 1.0f / 60.0f;
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		finished_ = true;
	}

	color_.w = std::clamp(1.0f - counter_ / kDuration, 0.0f, 1.0f);
	objectColor_.SetColor(color_);

	for (WorldTransform& worldTransform : worldTransforms_) {
		WorldMatrixUpdate(worldTransform);
	}
};

void DeathParticles::Draw() {
	if (finished_) {
		return;
	}

	for (WorldTransform& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, *camera_, &objectColor_);
	}
};