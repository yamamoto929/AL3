#include "GameScene.h"
#include <vector>

using namespace KamataEngine;

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

GameScene::~GameScene() {
	delete model_;
	delete modelBlock_;
	delete worldTransform_;
	delete camera_;
	delete player_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	delete debugCamera_;
}

// 初期化
void GameScene::Initialize() {
	textureHandle_ = TextureManager::Load("uvChecker.png");
	textureHandleBlock_ = TextureManager::Load("uvChecker.png");

	model_ = Model::Create();
	modelBlock_ = Model::Create();

	worldTransform_ = new KamataEngine::WorldTransform();
	worldTransform_->Initialize();

	camera_ = new KamataEngine::Camera();
	camera_->Initialize();

	player_ = new Player();
	player_->Initialize(model_, textureHandle_, camera_);

	// 要素数
	const uint32_t kNumBlockVirtical = 10;
	const uint32_t kNumBlockHorizontal = 20;
	// ブロック1個分の横幅
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;
	// 要素数を変更する
	// 列数を設定 (縦方向のブロック数)
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		// 1列の要素数を設定 (横方向のブロック数)
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			if (j % 2 == 0 && i % 2 == 1) {
				continue;
			} else if (j % 2 == 1 && i % 2 == 0) {
				continue;
			}
			worldTransformBlocks_[i][j] = new WorldTransform();
			worldTransformBlocks_[i][j]->Initialize();
			worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
			worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
		}
	}

	debugCamera_ = new DebugCamera(1280, 720);
};

// 更新処理
void GameScene::Update() {
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif
	player_->Update();
	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			/*アフィン変換行列の作成*/
			Matrix4x4 affineMatrix = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			worldTransformBlock->matWorld_ = affineMatrix;
			// 定数バッファに転送する
			worldTransformBlock->TransferMatrix();
		}
	}

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_->matView = debugCamera_->GetCamera().matView;
		camera_->matProjection = debugCamera_->GetCamera().matProjection;
		camera_->TransferMatrix();
	} else {
		camera_->UpdateMatrix();
	}
};

// 描画処理
void GameScene::Draw() {
	
	Model::PreDraw();
	// model_->Draw(worldTransform_, camera_, textureHandle_);
	// player_->Draw();
	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, *camera_);
		}
	}

	Model::PostDraw();
	
};

// アフィン変換行列を生成
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 result = {};
	// 回転行列の計算
	float sx = sinf(rotate.x), cx = cosf(rotate.x);
	float sy = sinf(rotate.y), cy = cosf(rotate.y);
	float sz = sinf(rotate.z), cz = cosf(rotate.z);

	// 回転行列
	Matrix4x4 matRot = {};
	matRot.m[0][0] = cy * cz;
	matRot.m[0][1] = cy * sz;
	matRot.m[0][2] = -sy;

	matRot.m[1][0] = sx * sy * cz - cx * sz;
	matRot.m[1][1] = sx * sy * sz + cx * cz;
	matRot.m[1][2] = sx * cy;

	matRot.m[2][0] = cx * sy * cz + sx * sz;
	matRot.m[2][1] = cx * sy * sz - sx * cz;
	matRot.m[2][2] = cx * cy;

	matRot.m[3][3] = 1.0f;

	// スケールを回転行列に適用
	for (int i = 0; i < 3; ++i) {
		result.m[0][i] = matRot.m[0][i] * scale.x;
		result.m[1][i] = matRot.m[1][i] * scale.y;
		result.m[2][i] = matRot.m[2][i] * scale.z;
	}

	// 平行移動を代入
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.0f;

	return result;
}