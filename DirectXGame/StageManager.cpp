#include "StageManager.h"
#include <cassert>
#include <fstream>
#include <sstream>

void StageManager::LoadStageFiles() {
	const std::string filepath = "Resources/stageDatas.csv";
	std::ifstream file;
	file.open(filepath);
	assert(file.is_open());

	std::stringstream mapChipCsv;
	mapChipCsv << file.rdbuf();
	file.close();

	std::string line;

	while (std::getline(mapChipCsv, line)) {
		std::stringstream lineStream(line);

		StageData stageData;

		std::string word;
		std::getline(lineStream, word, ',');

		stageData.name = word;

		std::getline(lineStream, word, ',');
		stageData.timeLimit = std::stoi(word);

		stageDatas_.push_back(stageData);
	}
}

/// <summary>
/// ステージ名指定で現在ステージ番号設定
/// </summary>
/// <param name="stageName">ステージ名</param>
void StageManager::SetCurrentStageIndexByName(const std::string& name) {
	// 全ステージデータを検索
	for (size_t i = 0; i < stageDatas_.size(); ++i) {
		// ステージ名が一致したら現在ステージ番号を設定する
		if (stageDatas_[i].name == name) {
			currentStageIndex_ = static_cast<int32_t>(i);
			// 目的を達したので関数を抜ける
			return;
		}
	}
	assert(false && "指定されたステージ名は存在しません");
}
