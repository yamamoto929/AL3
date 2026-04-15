#include "TitleScene.h"
#include "WorldMatrixUpdate.h"
#include <numbers>
using namespace KamataEngine;

TitleScene::~TitleScene() {
	delete modelPlayer_;
	delete modelTitleFont_;
	delete camera_;
	delete fade_;
};

void TitleScene::Initialize() { 
	modelPlayer_ = Model::CreateFromOBJ("player", true); 
	modelTitleFont_ = Model::CreateFromOBJ("titleFont", true);

	worldTransformPlayer_.Initialize();
	worldTransformPlayer_.rotation_.y = std::numbers::pi_v<float>;
	worldTransformPlayer_.translation_.y = kPlayerTranslationY;
	worldTransformPlayer_.scale_.x = kPlayerScale;
	worldTransformPlayer_.scale_.y = kPlayerScale;
	worldTransformPlayer_.scale_.z = kPlayerScale;

	worldTransformTitleFont_.Initialize();
	worldTransformTitleFont_.scale_.x = kTitleFontScale;
	worldTransformTitleFont_.scale_.y = kTitleFontScale;
	worldTransformTitleFont_.scale_.z = kTitleFontScale;
	
	worldTransformTitleFont_.translation_.y = kTitleFontTranslationY;

	camera_ = new Camera();
	camera_->Initialize();

	camera_->translation_.z = kCameraTranslationZ;

	fade_ = new Fade;
	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, kFadingTime);
};

void TitleScene::Update() { 
	switch (phase_) {
	case Phase::kMain:
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, kFadingTime);
		}
		break;

	case Phase::kFadeIn:
	case Phase::kFadeOut:
		fade_->Update();

		if (fade_->isFinished()) {
			if (phase_ == Phase::kFadeIn) {
				phase_ = Phase::kMain;
			} else {
				finished_ = true;
			}
		}
		break;
	}

	worldTransformTitleFont_.rotation_.y += kTitleFontRotationSpeedY;
	
	WorldMatrixUpdate(worldTransformPlayer_);
	WorldMatrixUpdate(worldTransformTitleFont_);

	camera_->UpdateMatrix();
}

void TitleScene::Draw() {
	
	Model::PreDraw();
	modelPlayer_->Draw(worldTransformPlayer_, *camera_);
	modelTitleFont_->Draw(worldTransformTitleFont_, *camera_);
	Model::PostDraw();

	if (phase_ == Phase::kMain) {
		return;
	}

	fade_->Draw();
};
