#pragma once
#include "KamataEngine.h"
#include "math/Vector3.h"
enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
	kPlayer, // プレイヤー
	kEnemy, // 敵
};

struct MapChipDataUnit {
	MapChipType type;
	uint8_t subID;
};

struct MapChipData {
	std::vector<std::vector<MapChipDataUnit>> data;
};

///< summary>
/// マップチップフィールド
/// </summary>
class MapChipField {
public:
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	struct Rect {
		float left; // 左端

		float right; // 右端

		float bottom; // 下端

		float top; // 上端
	};

	enum MapChipCharIndex {
		kChipType = 0, // マップチップタイプ
		kChipSubID = 1 // タイプごとのサブID
	};

private:
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	static inline const uint32_t kBlockVirtical = 20;    // 縦
	static inline const uint32_t kBlockHorizontal = 100; // 横

	MapChipData mapChipData_;

public:
	uint32_t GetBlockVirtical() { return kBlockVirtical; }
	uint32_t GetBlockHorizontal() { return kBlockHorizontal; }
	void ResetMapChipData();

	void LoadMapChipCsv(const std::string filepath);
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
	IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position);

	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);
	uint8_t GetMapChipSubIDByIndex(uint32_t xIndex, uint32_t yIndex);
};
