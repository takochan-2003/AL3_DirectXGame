#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Player.h"
#include "Enemy.h"
#include "EnemyBullet.h"
#include "Skydome.h"
#include "DebugCamera.h"
#include "RailCamera.h"

/// <summary>
/// ゲームシーン
/// </summary>



class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheakAllCollisions();

	/// <summary>
	/// 敵弾を追加しよう
	/// </summary>
	void AddEnemyBullet(EnemyBullet* enemyBullet);

	//敵の発生
	void EnemySpawn(Vector3 position, Vector3 velocity);

	//敵発生のデータの読み込み
	void LoadEnemyPopData();

	//敵発生のコマンドの更新
	void UpdateEnemyPopCommands();

	// 敵リスト
	std::list<Enemy*> enemys_;

	/////敵弾リストの変数
	// 弾
	std::list<EnemyBullet*> enemybullets_;

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	uint32_t textureHandle_ = 0;
	Model* model_ = nullptr;

	WorldTransform worldTransform_;
	ViewProjection viewProjection_;
	Player* player_ = nullptr;
	Enemy* enemy_ = nullptr;
	EnemyBullet* enemybullet_ = nullptr;
	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;
	//レールカメラをゲームシーンに持たせる
	RailCamera* railCamera_ = nullptr;
	Vector3 worldPos;
	Vector3 rotate;
	//デバッグカメラ有効
	bool isDebugCameraActive_ = false;
	//デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	
	// 速度
	Vector3 velocity_ = {0.0f};
	// 寿命
	static const int32_t kLifeTime = 60 * 5;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// ですフラグ
	bool isDead_ = false;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
