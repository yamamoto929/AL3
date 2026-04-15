#pragma once
#include "KamataEngine.h"
#include "Fade.h"
class TitleScene {
private:
	enum class Phase {
		kFadeIn,
		kMain,
		kFadeOut
	};

	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::Model* modelTitleFont_ = nullptr;

	//KamataEngine::WorldTransform worldTransform_;
	KamataEngine::WorldTransform worldTransformPlayer_;
	KamataEngine::WorldTransform worldTransformTitleFont_;
	KamataEngine::Camera* camera_;

	static const inline float kPlayerTranslationY = -2.0f;
	static const inline float kPlayerScale= 1.5f;
	static const inline float kTitleFontScale = 0.7f;
	static const inline float kTitleFontTranslationY = 2.0f;
	static const inline float kCameraTranslationZ = -15.0f;

	static const inline float kTitleFontRotationSpeedY = -0.03f;

	bool finished_ = false;

	Fade* fade_ = nullptr;

	static const inline float kFadingTime = 0.75f;

	Phase phase_ = Phase::kFadeIn;

public:
	~TitleScene();
	void Initialize();
	void Update();
	void Draw();
	bool IsFinished() const { return finished_; }
};
