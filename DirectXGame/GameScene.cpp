#include "GameScene.h"
#include "StageManager.h"
#include "WorldMatrixUpdate.h"
#include "imgui.h"
#include <vector>
using namespace KamataEngine;

GameScene::~GameScene() {
	delete modelPlayer_;
	delete modelBlock_;
	delete modelHitEffect_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	delete debugCamera_;
	delete camera_;
	delete worldTransform_;
	delete player_;
	delete skydome_;
	delete modelSkydome_;
	delete mapChipField_;
	delete cameraController_;
	for (BaseEnemy* enemy : enemies_) {
		delete enemy;
	}
	delete deathParticles_;

	for (BaseEffect* effect : effects_) {
		delete effect;
	}

	delete modelGuardEffect_;
}
// 初期化
void GameScene::Initialize(StageManager* stageDataManager) {
	stageManager_ = stageDataManager;
	textureHandle_ = TextureManager::Load("uvChecker.png");
	textureHandleBlock_ = TextureManager::Load("uvChecker.png");

	modelPlayer_ = Model::CreateFromOBJ("player", true);
	modelAttack_ = Model::CreateFromOBJ("hit_effect", true);

	modelBlock_ = Model::CreateFromOBJ("box", true);
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);
	modelShieldEnemy_ = Model::CreateFromOBJ("shieldEnemy", true);
	modelDeathParticles_ = Model::CreateFromOBJ("deathParticle", true);

	modelHitEffect_ = Model::CreateFromOBJ("particle");
	modelGuardEffect_ = Model::CreateFromOBJ("ring");

	worldTransform_ = new WorldTransform();
	worldTransform_->Initialize();

	camera_ = new Camera();
	camera_->Initialize();

	debugCamera_ = new DebugCamera(1280, 720);

	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	skydome_ = new Skydome();
	skydome_->Initialize(camera_, modelSkydome_);

	mapChipField_ = new MapChipField();
	stageManager_->SetCurrentStageIndex(1);
	const StageData& stageData = stageManager_->GetCurrentStageData();

	std::string stageFileName = "Resources/fields/" + stageData.name + ".csv";
	mapChipField_->LoadMapChipCsv(stageFileName);

	GenerateFieldObjects();

	player_->SetMapChipField(mapChipField_);

	deathParticles_ = new DeathParticles();
	deathParticles_->Initialize(modelDeathParticles_, camera_, Vector3{0.0f});

	// for (Enemy* enemy : enemies_) {
	// enemy = new Enemy();
	// enemy->Initialize(modelEnemy_, camera_, Vector3{10.0f, 1.0f, 0.0f});
	//}

	cameraController_ = new CameraController();
	cameraController_->Initialize(camera_);
	cameraController_->SetTarget(player_);
	CameraController::Rect movableArea = {11.0f, 88.0f, 6.0f, 20.0f};
	cameraController_->SetMovableArea(movableArea);
	cameraController_->Reset();

	phase_ = Phase::kFadeIn;

	fade_ = new Fade;
	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, kFadingTime);
};

// 更新処理
void GameScene::Update() {
#ifdef _DEBUG
	ImGui::Begin("HotReload");

	if (ImGui::Button("Reload")) {
		reloadRequested_ = true;
	}

	ImGui::End();
#endif
	//  天球
	skydome_->Update();

	switch (phase_) {
	case Phase::kFadeIn:
	case Phase::kPlay:
		player_->Update();
		UpdateEnemies();
		cameraController_->Update();

		// ヒットエフェクト削除
		effects_.remove_if([](BaseEffect* effect) {
			if (effect->IsDead()) {
				delete effect;
				return true;
			}
			return false;
		});

		for (BaseEffect* effect : effects_) {
			effect->Update();
		}

		UpdateCamera();
		UpdateBlocks();
		CheckAllCollisions();

		if (phase_ == Phase::kFadeIn) {
			fade_->Update();
		}
		break;

	case Phase::kDead:
	case Phase::kFadeOut:
		UpdateEnemies();

		if (phase_ == Phase::kDead) {
			UpdateDeathParticles();
		}

		effects_.remove_if([](BaseEffect* effect) {
			if (effect->IsDead()) {
				delete effect;
				return true;
			}
			return false;
		});

		for (BaseEffect* effect : effects_) {
			effect->Update();
		}

		UpdateCamera();
		UpdateBlocks();

		if (phase_ == Phase::kFadeOut) {
			fade_->Update();
		}
		break;
	}

	ChangePhase();
};

// 描画処理
void GameScene::Draw() {

	Model::PreDraw();
	// model_->Draw(worldTransform_, camera_, textureHandle_);
	player_->Draw();
	skydome_->Draw();

	for (BaseEnemy* enemy : enemies_) {
		enemy->Draw();
	}

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, *camera_);
		}
	}

	for (BaseEffect* effect : effects_) {
		effect->Draw();
	}

	if (deathParticles_ != nullptr) {
		deathParticles_->Draw();
	}

	Model::PostDraw();

	if (phase_ == Phase::kPlay || phase_ == Phase::kDead) {
		return;
	}

	fade_->Draw();
};

void GameScene::GenerateFieldObjects() {
	uint32_t numBlockVirtical = mapChipField_->GetBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetBlockHorizontal();
	// 要素数を変更する
	// 列数を設定 (縦方向のブロック数)
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		// 1列の要素数を設定 (横方向のブロック数)
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	for (uint32_t vir = 0; vir < numBlockVirtical; ++vir) {
		for (uint32_t hor = 0; hor < numBlockHorizontal; ++hor) {
			switch (mapChipField_->GetMapChipTypeByIndex(hor, vir)) {
			case MapChipType::kBlock: {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[vir][hor] = worldTransform;
				worldTransformBlocks_[vir][hor]->translation_ = mapChipField_->GetMapChipPositionByIndex(hor, vir);
				break;
			}
			case MapChipType::kPlayer: {
				Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(hor, vir);

				player_ = new Player();
				player_->Initialize(modelPlayer_, modelAttack_, camera_, playerPosition);
				break;
			}
			case MapChipType::kEnemy: {
				uint8_t subID = mapChipField_->GetMapChipSubIDByIndex(hor, vir);
				switch (subID) {
				case 0: {
					Enemy* newEnemy = new Enemy();
					Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(hor, vir);
					newEnemy->Initialize(modelEnemy_, camera_, enemyPosition, this);
					enemies_.push_back(newEnemy);
					break;
				}
				case 1: {
					ShieldEnemy* newEnemy = new ShieldEnemy();
					Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(hor, vir);
					newEnemy->Initialize(modelShieldEnemy_, camera_, enemyPosition, this);
					enemies_.push_back(newEnemy);
					break;
				}
				default:
					break;
				}
				break;
			}
			default:
				break;
			}
		}
	}
}

void GameScene::CheckAllCollisions() {
#pragma region checkPlayer-EnemyCollision
	{
		AABB aabb1, aabb2;

		aabb1 = player_->GetAABB();

		for (BaseEnemy* enemy : enemies_) {
			if (enemy->IsCollidionDisabled()) {
				continue;
			}

			aabb2 = enemy->GetAABB();

			if (CheckAABBCollision(aabb1, aabb2)) {
				player_->OnCollision(enemy);
				enemy->OnCollision(player_);
			}
		}
	}
#pragma endregion
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kPlay:
		if (player_->IsDead()) {
			phase_ = Phase::kDead;
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			deathParticles_->Initialize(modelDeathParticles_, camera_, deathParticlesPosition);
		}
		break;
	case Phase::kDead:
		if (deathParticles_ && deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, kFadingTime);
		}
		break;
	case Phase::kFadeIn:
	case Phase::kFadeOut:
		if (fade_->isFinished()) {

			if (phase_ == Phase::kFadeIn) {
				phase_ = Phase::kPlay;
			} else {
				finished_ = true;
			}
		}
	}
}

void GameScene::UpdateEnemies() {
	enemies_.remove_if([](BaseEnemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

	for (BaseEnemy* enemy : enemies_) {
		enemy->Update();
	}
};

void GameScene::UpdateCamera() {
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_->matView = debugCamera_->GetCamera().matView;
		camera_->matProjection = debugCamera_->GetCamera().matProjection;
		camera_->TransferMatrix();
	} else {
		camera_->UpdateMatrix();
	}
};

void GameScene::UpdateBlocks() {
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			WorldMatrixUpdate(*worldTransformBlock);
		}
	}
};

void GameScene::UpdateDeathParticles() {
	/*if (deathParticles_ && deathParticles_->IsFinished()) {
	    finished_ = true;
	}*/

	if (deathParticles_ != nullptr) {
		deathParticles_->Update();
	}
}

void GameScene::CreateHitEffect(KamataEngine::Vector3 spawnPoint) {
	HitEffect* newHitEffect = HitEffect::Create(spawnPoint, modelHitEffect_, camera_);
	effects_.push_back(newHitEffect);
}

void GameScene::CreateGuardEffect(KamataEngine::Vector3 spawnPoint) {
	GuardEffect* newGuardEffect = GuardEffect::Create(spawnPoint, modelGuardEffect_, camera_);
	effects_.push_back(newGuardEffect);
}