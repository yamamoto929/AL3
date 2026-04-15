#define NOMINMAX
#include "Player.h"
#include "Easing.h"
#include "Vector3Operator.h"
#include "WorldMatrixUpdate.h"
#include <algorithm>
#include <cassert>
#include <numbers>
#include "2d/ImGuiManager.h"

using namespace KamataEngine;

void Player::Initialize(Model* model, KamataEngine::Model* modelAttack,Camera* camera, const Vector3& position) {
	assert(model);
	model_ = model;
	modelAttack_ = modelAttack;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	worldTransformAttack_.Initialize();
	// worldTransform_.translation_.y = 1.0f;
	//  worldTransform_.translation_.x += 2.0f;
	camera_ = camera;
}

void Player::Update() {

	if (isKnockbackRequested_) {
		behaviorRequest_ = Behavior::kKnockback;
		isKnockbackRequested_ = false;
	}
	
	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Behavior::kKnockback:
			BehaviorKnockbackInitialize();
			break;
		}

		behaviorRequest_ = Behavior::kUnknown;
	}

	

	switch (behavior_) {
	case Behavior::kRoot:

		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Behavior::kKnockback:
		BehaviorKnockbackUpdate();
		break;
	}

	// 旋回制御
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;

		float t = 1.0f - (turnTimer_ / kTimeTurn);

		float easedT = 1.0f - powf(1.0f - t, 5.0f);
		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		// 状態に応じた目標角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = turnFirstRotationY_ + (destinationRotationY - turnFirstRotationY_) * easedT;
	}

	WorldMatrixUpdate(worldTransform_);
	WorldMatrixUpdate(worldTransformAttack_);
}

void Player::BehaviorRootUpdate() {
	isAttack_ = false;
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {

		if (canRush_) {
			behaviorRequest_ = Behavior::kAttack;
			canRush_ = false;
		}
	}

	Move();

	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.velocity = velocity_;

	CheckMapCollidion(collisionMapInfo);

	MoveByResult(collisionMapInfo);

	OnContactCeiling(collisionMapInfo);

	GroundedStatusHandling(collisionMapInfo);

	OnContactWall(collisionMapInfo);
};

void Player::BehaviorAttackUpdate() {
	isAttack_ = true;
	movingAttackCount_ += 1.0f / 60.0f;

	Vector3 velocity{};
	switch (attackPhase_) {
	case AttackPhase::kPrepare:
	default: {
		float t = movingAttackCount_ / kPrepareTime;
		worldTransform_.scale_.z = EaseOut(1.0f, 0.07f, t);
		worldTransform_.scale_.y = EaseOut(1.0f, 1.5f, t);

		// 突進移行
		if (movingAttackCount_ >= kPrepareTime) {
			attackPhase_ = AttackPhase::kRush;
			movingAttackCount_ = 0.0f;
		}
		break;
	}
	case AttackPhase::kRush: {
		float t = movingAttackCount_ / kRushTime;
		worldTransform_.scale_.z = EaseOut(0.07f, 1.7f, t);
		worldTransform_.scale_.y = EaseIn(1.5f, 0.1f, t);

		if (lrDirection_ == LRDirection::kRight) {
			velocity.x = +kAttackVelocity;
		} else {
			velocity.x = -kAttackVelocity;
		}

		// 余韻移行
		if (movingAttackCount_ >= kRushTime) {
			attackPhase_ = AttackPhase::kRecovery;
			movingAttackCount_ = 0.0f;
		}
		break;
	}
	case AttackPhase::kRecovery: {
		float t = movingAttackCount_ / kAttackRecoveryTime;
		worldTransform_.scale_.z = EaseOut(1.5f, 1.0f, t);
		worldTransform_.scale_.y = EaseOut(0.5f, 1.0f, t);

		if (movingAttackCount_ >= kAttackRecoveryTime) {
			behaviorRequest_ = Behavior::kRoot;
		}

		break;
	}
	}

	worldTransformAttack_.translation_ = worldTransform_.translation_;
	worldTransformAttack_.rotation_= worldTransform_.rotation_;

	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.velocity = velocity;

	CheckMapCollidion(collisionMapInfo);

	MoveByResult(collisionMapInfo);

	OnContactCeiling(collisionMapInfo);

	GroundedStatusHandling(collisionMapInfo);

	OnContactWall(collisionMapInfo);
};

void Player::BehaviorKnockbackUpdate() {
	knockbackCount_ += 1.0f / 60.0f;
	Vector3 velocity = {0.0f};
	switch (knockbackPhase_) {
	case KnockbackPhase::kLaunched:{
		if (lrDirection_ == LRDirection::kRight) {
			velocity.x = -kKnockbackVelocity;
		} else {
			velocity.x = +kKnockbackVelocity;
		}

		// 突進移行
		if (knockbackCount_ >= kKnockbackLaunchedTime_) {
			knockbackCount_ = 0.0f;
			knockbackPhase_ = KnockbackPhase::kRecovery;
		}
		break;
	}
	case KnockbackPhase::kRecovery: {

		// 余韻移行
		if (knockbackCount_ >= kKnockbackRecoveryTime_) {
			behaviorRequest_ = Behavior::kRoot;
		}
		break;
	}
	}
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.velocity = velocity;

	CheckMapCollidion(collisionMapInfo);

	MoveByResult(collisionMapInfo);

	OnContactCeiling(collisionMapInfo);

	GroundedStatusHandling(collisionMapInfo);

	OnContactWall(collisionMapInfo);
};

void Player::Draw() {
	if (isDead_) {
		return;
	}
	model_->Draw(worldTransform_, *camera_);

	if (behavior_ == Behavior::kAttack && attackPhase_ == AttackPhase::kRush) {
		modelAttack_->Draw(worldTransformAttack_, *camera_);
	}
}

void Player::Move() {
	if (onGround_) {
		// 移動入力
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {

				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {

				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x -= kAcceleration;

				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}
			// 加速/減速
			velocity_ += acceleration;

			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}

		if (Input::GetInstance()->PushKey(DIK_UP)) {	
				// ジャンプ初速
				velocity_ += Vector3(0, kJumpAcceleration, 0);	
		}
	} else {
		// 落下速度
		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, kLimitFallSpeed);
	}
}

void Player::CheckMapCollidion(CollisionMapInfo& info) {
	CheckMapCollidionTop(info);
	CheckMapCollidionBottom(info);
	CheckMapCollidionLeft(info);
	CheckMapCollidionRight(info);
}

// 上
void Player::CheckMapCollidionTop(CollisionMapInfo& info) {
	if (info.velocity.y <= 0.0f) {
		return;
	}

	std::array<Vector3, kNUMCORNER> positionsNew{};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.velocity, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLEFTTOP]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRIGHTTOP]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除するために移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLEFTTOP]);
		MapChipField::IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(CornerPosition(worldTransform_.translation_, kLEFTTOP));

		if (indexSet.yIndex != indexSetNow.yIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.velocity.y = std::max(0.0f, (rect.bottom - worldTransform_.translation_.y) - (kHeight / 2.0f + kBlank));

			info.isCollideCeiling = true;
		}
	}
};

// 下
void Player::CheckMapCollidionBottom(CollisionMapInfo& info) {
	if (info.velocity.y >= 0.0f) {
		return;
	}

	std::array<Vector3, kNUMCORNER> positionsNew{};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.velocity, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	bool hit = false;
	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLEFTBOTTOM]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRIGHTBOTTOM]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除するために移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLEFTBOTTOM]);
		MapChipField::IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(CornerPosition(worldTransform_.translation_, kLEFTBOTTOM));

		if (indexSet.yIndex != indexSetNow.yIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.velocity.y = std::min(0.0f, (rect.top - worldTransform_.translation_.y) + (kHeight / 2.0f + kBlank));
			info.isLanding = true;
		}
	}
};

// 左
void Player::CheckMapCollidionLeft(CollisionMapInfo& info) {
	if (info.velocity.x >= 0.0f) {
		return;
	}
	std::array<Vector3, kNUMCORNER> positionsNew{};
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.velocity, static_cast<Corner>(i));
	}
	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 左側の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLEFTTOP]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLEFTBOTTOM]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除するために移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLEFTTOP]);
		MapChipField::IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(CornerPosition(worldTransform_.translation_, kLEFTTOP));

		if (indexSet.xIndex != indexSetNow.xIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.velocity.x = std::max(0.0f, (rect.right - worldTransform_.translation_.x) - (kWidth / 2.0f + kBlank));
			info.isTouchingWall = true;
		}
	}
};

// 右
void Player::CheckMapCollidionRight(CollisionMapInfo& info) {
	if (info.velocity.x <= 0.0f) {
		return;
	}

	std::array<Vector3, kNUMCORNER> positionsNew{};
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.velocity, static_cast<Corner>(i));
	}
	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 右側の当たり判定を行う
	bool hit = false;
	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRIGHTTOP]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRIGHTBOTTOM]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除するために移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRIGHTTOP]);
		MapChipField::IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(CornerPosition(worldTransform_.translation_, kRIGHTTOP));

		if (indexSet.xIndex != indexSetNow.xIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.velocity.x = std::min(0.0f, (rect.left - worldTransform_.translation_.x) + (kWidth / 2.0f + kBlank));
			info.isTouchingWall = true;
		}
	}
};
Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNUMCORNER] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0.0f}, // kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0.0f}, // kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0.0f}, // kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0.0f}  // kLeftTop
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
};

void Player::MoveByResult(const CollisionMapInfo& info) { worldTransform_.translation_ += info.velocity; };

void Player::OnContactCeiling(const CollisionMapInfo& info) {
	if (info.isCollideCeiling) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0.0f;
	}
};

void Player::GroundedStatusHandling(const CollisionMapInfo& info) {
	if (onGround_) {

		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			MapChipType mapChipType;
			// 真下の当たり判定を行う
			bool hit = false;

			// 左下点の判定
			MapChipField::IndexSet indexSetLeftBottom;
			indexSetLeftBottom = mapChipField_->GetMapChipIndexSetByPosition(CornerPosition(worldTransform_.translation_, kLEFTBOTTOM) + Vector3{0.0f, -kGroundSnapOffset, 0.0f});
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSetLeftBottom.xIndex, indexSetLeftBottom.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 右下点の判定
			MapChipField::IndexSet indexSetRightBottom;
			indexSetRightBottom = mapChipField_->GetMapChipIndexSetByPosition(CornerPosition(worldTransform_.translation_, kRIGHTBOTTOM) + Vector3{0.0f, -kGroundSnapOffset, 0.0f});
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSetRightBottom.xIndex, indexSetRightBottom.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			if (!hit) {
				onGround_ = false;
			} else {
				canRush_ = true;
			}
		}

	} else {

		if (info.isLanding) {
			onGround_ = true;

			canRush_ = true;

			velocity_.x *= (1.0f - kAttenuationLanding);

			velocity_.y = 0.0f;
		}
	}
};

void Player::OnContactWall(const CollisionMapInfo& info) {
	if (info.isTouchingWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
};

Vector3 Player::GetWorldPosition() {
	Vector3 worldPos{};

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
};

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb{};
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth};

	return aabb;
};

void Player::OnCollision(const BaseEnemy* enemy) {
	if (IsAttack()) {
		return;
	}
	(void)enemy;
	isDead_ = true;
};

void Player::BehaviorRootInitialize() {
	
};

void Player::BehaviorAttackInitialize() {
	// カウンタ初期化
	movingAttackCount_ = 0.0f;
	velocity_ = Vector3{0.0f, 0.0f, 0.0f};
	attackPhase_ = AttackPhase::kPrepare;
};

void Player::BehaviorKnockbackInitialize() {
	knockbackCount_=0.0f;
	velocity_ = Vector3{0.0f, 0.0f, 0.0f};
	worldTransform_.scale_ = Vector3{1.0f, 1.0f, 1.0f};
	knockbackPhase_ = KnockbackPhase::kLaunched;
};

void Player::KnockbackRequest() {
	isKnockbackRequested_=true;
};