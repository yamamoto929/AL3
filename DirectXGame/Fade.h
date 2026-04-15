#pragma once
#include "KamataEngine.h"
class Fade {
public:
	enum class Status { 
		None, 
		FadeIn,
		FadeOut 
	};

private:
	
	uint32_t textureHandle_ = 0u;
	KamataEngine::Sprite* sprite_ = nullptr;
	Status status_ = Status::None;
	// フェードの持続時間
	float duration_ = 0.0f;
	// 経過時間
	float counter_ = 0.0f;

	

public:
	void Initialize();
	void Update();
	void Draw();
	// フェード開始
	void Start(Status status, float duration);
	void Stop();

	bool isFinished() const;
};
