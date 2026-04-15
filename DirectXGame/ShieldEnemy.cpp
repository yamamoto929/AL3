#include "ShieldEnemy.h"
#include "Easing.h"
#include "GameScene.h"
#include "Player.h"
#include "Vector3Operator.h"
#include "WorldMatrixUpdate.h"
#include <cassert>
#include <numbers>

using namespace KamataEngine;
void ShieldEnemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, GameScene* gameScene) {
	assert(model);
	textureHandle_ = TextureManager::Load("uvChecker.png");
	model_ = model;
	gameScene_ = gameScene;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	camera_ = camera;
	velocity_ = Vector3{-kMoveSpeed, 0.0f, 0.0f};
	walkTimer_ = 0.0f;
	lrDirection_ = LRDirection::kLeft;
};

void ShieldEnemy::Update() {
	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {
		case Behavior::kWalk:
		default:
			BehaviorWalkInitialize();
			break;
		case Behavior::kDeath:
			BehaviorDeathInitialize();
			break;
		case Behavior::kKnockback:
			BehaviorKnockbackInitialize();
			break;
		}

		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case Behavior::kWalk:

		BehaviorWalkUpdate();
		break;
	case Behavior::kDeath:
		BehaviorDeathUpdate();
		break;
	case Behavior::kKnockback:
		BehaviorKnockbackUpdate();
		break;
	}
};

void ShieldEnemy::Draw() { model_->Draw(worldTransform_, *camera_); };

Vector3 ShieldEnemy::GetWorldPosition() {
	Vector3 worldPos{};

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
};

AABB ShieldEnemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb{};
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth};

	return aabb;
};

void ShieldEnemy::OnCollision(Player* player) {
	if (behavior_ == Behavior::kDeath) {
		return;
	}

	if (player->IsAttack()) {

		if ((player->GetLRDirection() == Player::LRDirection::kLeft && lrDirection_ == LRDirection::kRight) ||
		    (player->GetLRDirection() == Player::LRDirection::kRight && lrDirection_ == LRDirection::kLeft)) {
			gameScene_->CreateGuardEffect(worldTransform_.translation_);

			player->KnockbackRequest();
			behaviorRequest_ = Behavior::kKnockback;
			return;
		}

		behaviorRequest_ = Behavior::kDeath;

		Vector3 effectPos = (worldTransform_.translation_ + player->GetWorldTransform().translation_) / 2.0f;
		gameScene_->CreateHitEffect(effectPos);
	}
};

void ShieldEnemy::BehaviorWalkInitialize() {

};

void ShieldEnemy::BehaviorWalkUpdate() {
	worldTransform_.translation_ += velocity_;
	walkTimer_ += 1.0f / 60.0f;
	float param = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer_ / kWalkMotionTime);
	float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.y = degree * std::numbers::pi_v<float> / 180.0f;
	WorldMatrixUpdate(worldTransform_);
};

void ShieldEnemy::BehaviorDeathInitialize() {
	deathAnimTimer_ = 0.0f;
	isCollidionDisabled_ = true;
};

void ShieldEnemy::BehaviorDeathUpdate() {
	deathAnimTimer_ += 1.0f / 60.0f;
	float t = deathAnimTimer_ / kDeathAnimTimerMax_;
	worldTransform_.rotation_.y = EaseIn(std::numbers::pi_v<float> * 3.0f / 2.0f, -std::numbers::pi_v<float> * 1.0f / 2.0f, t);
	worldTransform_.rotation_.x = EaseIn(0.0f, -std::numbers::pi_v<float> * 1.0f / 2.0f, t);
	WorldMatrixUpdate(worldTransform_);
	if (deathAnimTimer_ >= kDeathAnimTimerMax_) {
		isDead_ = true;
	}
};

void ShieldEnemy::BehaviorKnockbackInitialize() {
	knockbackAnimTime_ = 0.0f;

	worldTransform_.rotation_.x = 0.0f;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	worldTransform_.rotation_.z = 0.0f;
};

void ShieldEnemy::BehaviorKnockbackUpdate() {

	knockbackAnimTime_ += 1.0f / 60.0f;

	// 進行度合いを 0.0 〜 1.0 で算出
	float t = knockbackAnimTime_ / kKnockbackAnimTimeMax;

	// 1.0を超えないようにクランプ（安全対策）
	if (t > 1.0f) {
		t = 1.0f;
	}

	// 半分のサイン波 (tが0〜1のとき、paramは 0 -> 1 -> 0 と変化する)
	float param = std::sin(std::numbers::pi_v<float> * t);

	// 0.0f からスタートし、最大 -30.0f まで行って 0.0f に戻る
	float degree = kKnockbackMotionAngleMax * param;

	// ラジアンに変換して代入
	worldTransform_.rotation_.x = degree * std::numbers::pi_v<float> / 180.0f;
	WorldMatrixUpdate(worldTransform_);

	// アニメーション終了で歩きに戻る
	if (knockbackAnimTime_ >= kKnockbackAnimTimeMax) {
		// 念のため完全に0.0fに戻しておく
		worldTransform_.rotation_.x = 0.0f;
		WorldMatrixUpdate(worldTransform_);

		behaviorRequest_ = Behavior::kWalk;
	}
};