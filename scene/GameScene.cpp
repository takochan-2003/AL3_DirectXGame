#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "ImGuiManager.h"
#include "AxisIndicator.h"
#include <fstream>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete player_;
	delete debugCamera_;
	delete modelSkydome_;
	delete railCamera_;

	for (Enemy* enemy : enemys_) {
		delete enemy;
	}

	for (EnemyBullet* enemyBullet : enemyBullets_) {
		delete enemyBullet;
	}
}


void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	textureHandle_ = TextureManager::Load("tennninnka.png");

	model_ = Model::Create();

	worldTransform_.Initialize();
	viewProjection_.Initialize();
	//自キャラの生成
	player_ = new Player();
	Vector3 playerPosition(0, 0, -1);
	player_->Initialize(model_,textureHandle_,playerPosition);
	//敵の速度
	const float kEnemySpeedX = 0.1f;
	const float kEnemySpeedY = 0.1f;
	const float kEnemySpeedZ = 0.1f;
	//敵の移動
	Vector3 velocity(kEnemySpeedX, kEnemySpeedY, kEnemySpeedZ);
	Vector3 EnemyPosition = {200.0f, 1000.0f, 40.0f};

	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, textureHandle_);

	//レールカメラ
	railCamera_ = new RailCamera();
	railCamera_->Initialize(worldPos,rotate);

	debugCamera_ = new DebugCamera(100, 100);

	//自キャラとレールカメラの親子関係を結ぶ
	player_->Setparent(&railCamera_->GetWorldTransform());

	AxisIndicator::GetInstance()->SetVisible(true);
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
	
	LoadEnemyPopData();
}

void GameScene::Update() { 

	railCamera_->Update();

	debugCamera_->Update();

	skydome_->Update();
	GameScene::CheakAllCollisions();
	#ifdef _DEBUG
	if (input_->TriggerKey(DIK_P)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
	#endif;

	

	//カメラの処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		//ビュープロジェクション行列の更新
		viewProjection_.TransferMatrix();
	} else {
	//ビュープロジェクション行列の更新と転送
		viewProjection_.matView = railCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の更新
		viewProjection_.TransferMatrix();
	}
	//敵キャラの更新、スポーンも入ってる
	UpdateEnemyPopCommands();

	//自キャラの更新
	player_->Update();


	//敵キャラの更新（リスト）
	for (Enemy* enemy : enemys_) {
		enemy->Update();
	}

	//弾更新
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Update();
	}

		// デスフラグの立った弾を削除
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});
\

}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	//自キャラの描画
	player_->Draw(viewProjection_);
	//敵キャラの描画
	for (Enemy* enemy : enemys_) {
		enemy->Draw(viewProjection_);
	}
	//弾の描画
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw(viewProjection_);
	}

	skydome_->Draw(viewProjection_);


	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

	

#pragma endregion
}

void GameScene::EnemySpawn(Vector3 position, Vector3 velocity) {
	Enemy* enemy = new Enemy();
	//初期化
	enemy->Initialize(model_, position, velocity);
	//敵キャラに自キャラのアドレスを渡す
	enemy->SetPlayer(player_);
	//敵キャラにゲームシーンを渡す
	enemy->SetGameScene(this);
	//リストに登録
	enemys_.push_back(enemy);
}


void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	//リストに登録する
	enemyBullets_.push_back(enemyBullet);

}

void GameScene::LoadEnemyPopData() {
	//ファイルを開く
	std::ifstream file;
	file.open("./Resources/enemyPop.csv");
	assert(file.is_open());

	//ファイルの内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();

	//ファイルを閉じる
	file.close();
}

void GameScene::UpdateEnemyPopCommands() {
	//待機処理
	if (enemyPopWaitFlag) {
		enemyPopWaitTimer--;
		if (enemyPopWaitTimer <= 0) {
		//待機完了
			enemyPopWaitFlag = false;
		}
		return;
	}

	//1行分の文字列を入れる変数
	std::string line;

	//コマンド実行ループ
	while (getline(enemyPopCommands, line)) {
	
	//1行分のもおじ列をストリームに変換して解析しやすくなる
		std::istringstream line_stream(line);
	
	    std::string word;
		//,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

	    // "//"から始まる行はコメント
		if (word.find("//") == 0) {
			continue;
		}

		//POPコマンド
		if (word.find("POP") == 0) {
			//x座標
			getline(line_stream,word, ',');
			float x = (float)std::atof(word.c_str());

			//y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			//z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			//敵を発生させる
			EnemySpawn(Vector3(x, y, z), {0.0f, 0.0f, 0.0f});
		}
		//WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			//待ち時間
			int32_t waitTime = atoi(word.c_str());

			//待機時間
			enemyPopWaitFlag = true;
			enemyPopWaitTimer = waitTime;

			//コマンドループを抜ける
			break;
		}
	
	}
}

void GameScene::CheakAllCollisions() {
	//判定対象AとBの座標
	Vector3 posA, posB, posC, posD;

	//2間点の距離(自キャラと敵弾の当たり判定)
	float posAB;

	//自キャラの半径
	float playerRadius = 40.0f;
	//敵弾の半径
	float enemyBulletRadius = 30.0f;

	//自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	//敵弾リストの取得
	//const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

	#pragma region 自キャラと敵弾の当たり判定
	//自キャラのワールド座標
	posA = player_->GetWorldPosition();

	//自キャラと敵弾すべての当たり判定
	for (EnemyBullet* bullet : enemyBullets_) {
	//敵弾の座標
		posB = bullet->GetWorldPosition();
		//AとBの距離を求める
		posAB=
		(posB.x - posA.x) * (posB.x - posA.x) +
		(posB.y - posA.y) * (posB.y - posA.y) +
		(posB.z - posA.z) * (posB.z - posA.z);
		//球と球の当たり判定
		if (posAB <= (playerRadius + enemyBulletRadius) * (playerRadius + enemyBulletRadius))
		{
			//自キャラの衝突時コールバックを呼び出す
			player_->OnCollision();
			//敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
			
		}
	}
	#pragma endregion

	#pragma region 自弾と敵キャラの当たり判定
	// 自弾の半径
	float playerBulletRadius = 3.0f;
	// 敵キャラの半径
	float enemyRadius = 30.0f;

	

	// 2間点の距離(自弾と敵キャラの当たり判定)
	float posCD;
	for (Enemy* enemy : enemys_) {
		// 敵キャラのワールド座標
		posC = enemy->GetWorldPosition();

		// 自弾と敵キャラの当たり判定
		for (PlayerBullet* bullet : playerBullets) {
			// 自弾の座標
			posD = bullet->GetWorldPosition();
			// AとBの距離を求める
			posCD = (posC.x - posD.x) * (posC.x - posD.x) + (posC.y - posD.y) * (posC.y - posD.y) +
			        (posC.z - posD.z) * (posC.z - posD.z);

			// 球と球の当たり判定
			if (posCD <= (playerBulletRadius + enemyRadius) * (playerBulletRadius + enemyRadius)) {
				// 自弾の衝突時コールバックを呼び出す
				enemy_->OnCollision();
				// 敵キャラの衝突時コールバックを呼び出す
				bullet->OnCollision();
			}
		}
	}
	#pragma endregion

	#pragma region 自弾と敵弾の当たり判定
	float posBD;
	for (PlayerBullet* playerbullet : playerBullets) {
		for (EnemyBullet* enemybullet : enemyBullets_) {
			posBD = (posB.x - posD.x) * (posB.x - posD.x) +
				    (posB.y - posD.y) * (posB.y - posD.y) +
			        (posB.z - posD.z) * (posB.z - posD.z);
			// 球と球の当たり判定
			if (posBD <= (playerBulletRadius + enemyBulletRadius) * (playerBulletRadius + enemyBulletRadius)) {
				// 自弾の衝突時コールバックを呼び出す
				enemybullet->OnCollision();
				// 敵キャラの衝突時コールバックを呼び出す
				playerbullet->OnCollision();
			}
		}
	}
	#pragma endregion
}
