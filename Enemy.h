#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include"Input.h"

class Enemy {

	public:

	void Initialize(Model* model, uint32_t textureHndle);

	void Update();

	void Draw(ViewProjection& viewProjection);

	private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	Enemy* enemy_ = nullptr;
	Input* input_ = nullptr;
};