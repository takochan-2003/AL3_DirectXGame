#include "Player.h"
#include <cassert>
#define _USE_MATH_DEFINES
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "keisan.h"
#include <math.h>

Player::~Player() {
	delete sprite2DReticle_;
}

void Player::Initialize(Model* model, uint32_t textureHndle,Vector3 playerPotision) {
	assert(model);
	model_ = model;
	
	textureHandle_ = textureHndle;

	worldTransform_.Initialize();
	input_ = Input::GetInstance();
	worldTransform_.scale_ = {3.0f, 3.0f, 3.0f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.translation_ = playerPotision;
	//worldTransform_.translation_ = {0.0f, 0.0f, 40.0f};
	//解放
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}

	//3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();

	//レティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("Reticle.png");


	Vector2 ReticlePos = {0.0f, 0.0f};
	Vector4 Color = {(1), (1), (1), (1)};
	Vector2 unker = {0.5f, 0.5f};
	//スプライト生成
	sprite2DReticle_ = Sprite::Create(textureReticle, ReticlePos, Color, unker);

}

void Player::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
	model_->Draw(worldTransform3DReticle_, viewProjection);

	//if (bullet_) {
	//	bullet_->Draw(viewProjection);
	//}
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

void Player::DrawUI() {
	sprite2DReticle_->Draw(); 
}

void Player::Update(ViewProjection& viewProjection) {

	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	Vector3 move = {0, 0, 0};
	const float kCharacterSpeed = 0.2f;

		// ゲームパッドの状態を得る変数
	XINPUT_STATE joyState;

	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
		move.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;
	}

	const float kRotSpeed = 0.02f;

	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y += kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	}

	float playerPos[3] = {
	    worldTransform_.translation_.x, worldTransform_.translation_.y,
	    worldTransform_.translation_.z};

	

	worldTransform_.translation_.x = playerPos[0];
	worldTransform_.translation_.y = playerPos[1];
	worldTransform_.translation_.z = playerPos[2];

	worldTransform_.translation_ = Add(worldTransform_.translation_, move);
	

	// 移動限界座標
	const float kMoveLimitX = 28;
	const float kMoveLimitY = 14;

	// 範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);
	
	worldTransform_.UpdateMatrix();

	//自機のワールド座標から3Dレティクルのワールド座標を計算
	//{
	//	//自機から3Dレティクルへの距離
	//	const float kDistancePlayerTo3DReticle = 50.0f;
	//	//自機から3Dレティクルへのオフセット（Z+向き）
	//	Vector3 offset = {0, 0, 1.0f};
	//	//自機のワールド行列の回転を反映
	//	offset = VectorMatrixMultiply(offset, worldTransform_.matWorld_);
	//	//ベクトルの長さを整える
	//	offset = VectorIndexMultiply(Normalize(offset),kDistancePlayerTo3DReticle);
	//	//3Dレティクルの座標を設定
	//	worldTransform3DReticle_.translation_ = Add(offset,GetWorldPosition());
	//	//ワールド行列の更新と転送
	//	
	//	//worldTransform3DReticle_.matWorld_ = MakeAffineMatrix(
	//	//    worldTransform3DReticle_.scale_,
	//	//	worldTransform3DReticle_.rotation_,
	//	//    worldTransform3DReticle_.translation_);
	//	
	//	worldTransform3DReticle_.UpdateMatrix();
	//}

	//3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	//{
	//	Vector3 positionReticle = {
	//	    worldTransform3DReticle_.matWorld_.m[3][0],
	//		worldTransform3DReticle_.matWorld_.m[3][1],
	//	    worldTransform3DReticle_.matWorld_.m[3][2]
	//	};
	//
	//	//ビューポート行列
	//	Matrix4x4 matViewport =
	//	    MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	//
	//	//ビュー行列とプロジェクション行列、ビューポート行列を合成する
	//	Matrix4x4 matViewProjectionViewport =
	//	    Multiply(Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);
	//
	//	//ワールド->スクリーン座標返還(ここで3Dから2Dになる)
	//	positionReticle = Transform(positionReticle, matViewProjectionViewport);
	//
	//	//スプライトのレティクルに座標設定
	//	//sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));
	//
	//}

	//マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクル配置
	{ 
		////ゲームパッドの状態を得る変数
		//XINPUT_STATE joyState;
		//
		////ゲームパッド状態取得
		//if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		//	move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
		//	move.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;
		//}

		//スプライトの現在座標を取得
		Vector2 spritePosition = sprite2DReticle_->GetPosition();

		//XINPUT_STATE joyState;

		//ジョイスティック状態取得
		if (Input::GetInstance()->GetJoystickState(0, joyState)) {
			spritePosition.x += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * 5.0f;
			spritePosition.y -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 5.0f;

			//スプライトの座標変更を反映
			sprite2DReticle_->SetPosition(spritePosition);
			
		}
		POINT mousePosition;
		//マウス座標(スクリーン座標)を計算する
		GetCursorPos(&mousePosition);

		//クライアントエリアに座標を変換する
		HWND hwnd = WinApp::GetInstance()->GetHwnd();
		ScreenToClient(hwnd, &mousePosition);

		//マウス座標を2Dレティクルのスプライトに大にゅする
		//sprite2DReticle_->SetPosition(Vector2((float)mousePosition.x, (float)mousePosition.y));

		// ビューポート行列
		Matrix4x4 matViewport2 =
		    MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

		//ビュープロジェクションビューポート合成行列
		Matrix4x4 matVPV =Multiply(Multiply(viewProjection.matView,
			viewProjection.matProjection), matViewport2);

		//合成行列の逆行列を計算する
		Matrix4x4 matInverseVPV = Inverse(matVPV);

		//スクリーン座標
		Vector3 posNear = Vector3((float)spritePosition.x, (float)spritePosition.y, 0);
		Vector3 posFar = Vector3((float)spritePosition.x, (float)spritePosition.y, 1);

		//スクリーン座標系からワールド座標系へ
		posNear = Transform(posNear, matInverseVPV);
		posFar = Transform(posFar, matInverseVPV);

		//マウス例の方向
		Vector3 mouseVector = Subtract(posFar, posNear);
		Vector3 mouseDirection = Add(posNear, mouseVector);

		mouseDirection = Normalize(mouseDirection);

		//カメラから照準オブジェクトの距離
		const float kDistanceTestObject = 50.0f;


		worldTransform3DReticle_.translation_ =
		    Add(posNear, VectorIndexMultiply(mouseDirection, kDistanceTestObject));

		worldTransform3DReticle_.UpdateMatrix();

		// 画面の座標を表示
		ImGui::Begin("Player");
		// ImGui::SliderFloat3("playerPos", playerPos, -28.0f, 28.0f);
		ImGui::Text("2DReticle:(%f,%f)", (float)mousePosition.x, (float)mousePosition.y);
		ImGui::Text("Near:(%+.2f,%+.2f,%+.2f)", posNear.x, posNear.y, posNear.z);
		ImGui::Text("Far:(%+.2f,%+.2f,%+.2f)", posFar.x, posFar.y, posFar.z);
		ImGui::Text(
		    "3DReticle:(%+.2f,%+.2f,%+.2f)", worldTransform3DReticle_.translation_.x,
		    worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);
        ImGui::End();
	}


	

	
	
	// 攻撃処理
	Attack();
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}
}

void Player::Rotate() {
	const float kRotSpeed = 0.02f;
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.x += kRotSpeed;
	}
}

void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {
		
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, 0);
		//向き
		//velocity = TransformNormal(velocity, worldTransform_.matWorld_);

		Vector3 world3D = {
		    worldTransform3DReticle_.matWorld_.m[3][0],
		    worldTransform3DReticle_.matWorld_.m[3][1],
		    worldTransform3DReticle_.matWorld_.m[3][2],
		};
		//velocity = VectorSubtract(worldTransform3DReticle_.translation_,worldTransform_.translation_);
		velocity =VectorSubtract(world3D, GetWorldPosition());
		velocity = VectorIndexMultiply(Normalize(velocity),kBulletSpeed);
		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, GetWorldPosition(), velocity);
		// 弾を登録する
		//bullet_ = newBullet;
 		bullets_.push_back(newBullet);
	}

	XINPUT_STATE joyState;

	//ゲームパッド未接続なら何もせず抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	//Rトリガーを押していたら
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, 0);
		// 向き
		// velocity = TransformNormal(velocity, worldTransform_.matWorld_);

		Vector3 world3D = {
		    worldTransform3DReticle_.matWorld_.m[3][0],
		    worldTransform3DReticle_.matWorld_.m[3][1],
		    worldTransform3DReticle_.matWorld_.m[3][2],
		};
		// velocity =
		// VectorSubtract(worldTransform3DReticle_.translation_,worldTransform_.translation_);
		velocity = VectorSubtract(world3D, GetWorldPosition());
		velocity = VectorIndexMultiply(Normalize(velocity), kBulletSpeed);
		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, GetWorldPosition(), velocity);
		// 弾を登録する
		// bullet_ = newBullet;
		bullets_.push_back(newBullet);
	}
}

Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::OnCollision() {}

//親子関係を結ぶ
void Player::Setparent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }

//
