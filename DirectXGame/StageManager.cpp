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
