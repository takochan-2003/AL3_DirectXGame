#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include"Input.h"
#include"PlayerBullet.h"


/// <summary>
/// 自キャラ
/// </summary>
class Player {

	public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model,uint32_t textureHndle);

	/// <summary>
	/// 自キャラ
	///更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection& viewProjection);

	void Rotate();

	void Attack();

	private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	Player* player_ = nullptr;
	Input* input_ = nullptr;
	float inputFloat[3] = {0, 0, 0};
	// 弾
	PlayerBullet* bullet_ = nullptr;

};