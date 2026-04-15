#pragma once
#include <string>
#include <vector>
#include <cassert>
struct StageData {
	std::string name; // ステージ名
	int32_t timeLimit; // 制限時間(秒)
};

class StageManager {
private:
	std::vector<StageData> stageDatas_;
	int32_t currentStageIndex_ = 0;

public:
	/// <summary>
	/// ステージデータファイルの読み込み
	/// </summary>
	void LoadStageFiles();

	/// <summary>
	/// ステージデータの取得
	/// </summary>
	/// <param name= "index">ステージ番号</param>
	/// <returns>ステージデータ</returns>
	const StageData& GetStageData(int32_t index) const {
		assert(index>=0&&index<=2);
		return stageDatas_[index];
	}

	void SetCurrentStageIndex(int32_t index) {
		assert(index>=0&&index<=2);
		currentStageIndex_ = index;
	}

	int32_t GetCurrentStageIndex() const { return currentStageIndex_; }

	/// <summary>
	///	現在ステージのステージデータ取得
	/// </summary>
	/// <param name= "index">ステージ番号</param>
	/// <returns>ステージデータ</returns>
	const StageData& GetCurrentStageData() const {
		return GetStageData(currentStageIndex_);
	}
};
