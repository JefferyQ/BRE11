#include "Scene.h"

#include <general/Camera.h>
#include <input/Keyboard.h>
#include <managers/DrawManager.h>
#include <rendering/GlobalResources.h>
#include <rendering/shaders/lightPasses/DirLightPsData.h>
#include <utils/Assert.h>
#include <utils/Memory.h>
#include <utils/Utility.h>

using namespace DirectX;

namespace {
	const XMFLOAT2 sLightRotationRate(XM_PI / 4.0f, XM_PI / 4.0f);


	const unsigned int sMaxShaderPointLights = 512;
	const unsigned int sNumPointLightShaders = 8;

	const char* sSceneModelsFile = "content\\configs\\fullyDeferred\\models.yml";
}

Scene::Scene() {
	InitDirectionalLights();
	InitPointLights();

	BRE::DrawManager::gInstance->LoadModels(sSceneModelsFile);
}

void Scene::Update(const float elapsedTime) {
	//UpdateDirectionalLight(elapsedTime);
	mPosUpdater.Update(elapsedTime);

	//
	// Update constant buffers
	//

	// Directional
	std::vector<BRE::LightsDrawer::DirLightData>& dirLightDataVec = BRE::DrawManager::gInstance->DirLightDataVec();
	for (BRE::LightsDrawer::DirLightData& data : dirLightDataVec) {
		data.mPixelShaderData.Light().mDirection = mDirectionalLight.Direction();
		XMStoreFloat3(&data.mPixelShaderData.CameraPosW(), BRE::Camera::gInstance->PositionVector());
	}

	// Point lights Pixel Shader
	std::vector<BRE::LightsDrawer::PointLightData>& quadCulledPointLightDataVec = BRE::DrawManager::gInstance->PointLightDataVec();
	for (BRE::LightsDrawer::PointLightData& data : quadCulledPointLightDataVec) {
		XMStoreFloat3(&data.mPointLightPsData.CameraPosW(), BRE::Camera::gInstance->PositionVector());
	}
}

void Scene::InitDirectionalLights() {
	std::vector<BRE::LightsDrawer::DirLightData>& dirLightDataVec = BRE::DrawManager::gInstance->DirLightDataVec();
	dirLightDataVec.resize(1);
	BRE::DirLightPixelShaderData& dirLightPsData = dirLightDataVec[0].mPixelShaderData;

	mDirectionalLight.SetColor(0.4f, 0.4f, 0.4f);
	mDirectionalLight.ApplyRotation(XMMatrixRotationX(XM_PI / -2.0f));

	BRE::DirectionalLightData& dirLightData = dirLightPsData.Light(); 
	dirLightData.mColor = mDirectionalLight.Color();
	dirLightData.mDirection = mDirectionalLight.Direction();

	dirLightPsData.SamplerState() = BRE::GlobalResources::gInstance->MinMagMipPointSampler(); 
}

void Scene::InitPointLights() { 
	std::vector<BRE::LightsDrawer::PointLightData>& quadCulledPointLightDataVec = BRE::DrawManager::gInstance->PointLightDataVec();
	quadCulledPointLightDataVec.resize(sNumPointLightShaders);
	unsigned int lightIndex = 0;
	for (BRE::LightsDrawer::PointLightData& data : quadCulledPointLightDataVec) {
		for (unsigned int iLight = 0; iLight < sMaxShaderPointLights; ++iLight, ++lightIndex) {
			data.mPointLightVsData.LightPosAndRadius(iLight).x = BRE::Utility::RandomFloat(-200.0f, 200.0f);
			data.mPointLightVsData.LightPosAndRadius(iLight).y = BRE::Utility::RandomFloat(-200.0f, 200.0f);
			data.mPointLightVsData.LightPosAndRadius(iLight).z = BRE::Utility::RandomFloat(-200.0f, 200.0f);
			data.mPointLightVsData.LightPosAndRadius(iLight).w = 20;
			const float c = BRE::Utility::RandomFloat(0.3f, 0.4f);
			DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(c, c, c, 0.0f);
			data.mPointLightVsData.LightColor(iLight) = color;

			const float f5 = BRE::Utility::RandomFloat(25.0f, 30.0f) * (rand() % 2 ? 1.0f : -1.0f);
			const float f6 = BRE::Utility::RandomFloat(25.0f, 30.0f) * (rand() % 2 ? 1.0f : -1.0f);
			const float f7 = BRE::Utility::RandomFloat(25.0f, 30.0f) * (rand() % 2 ? 1.0f : -1.0f);
			mPosUpdater.Add(BRE::PositionUpdater::Params(&data.mPointLightVsData.LightPosAndRadius(iLight), XMFLOAT3(-200.0f, -200.0f, -200.0f), XMFLOAT3(200.0f, 200.0f, 200.0f), XMFLOAT3(f5, f6, f7)));
		}
	}
}

void Scene::UpdateDirectionalLight(const float elapsedTime) {
	// Rotate directional light
	XMFLOAT2 rotationAmount(0.0f, 0.0f);
	if (BRE::Keyboard::gInstance->IsKeyDown(DIK_LEFTARROW)) {
		rotationAmount.x += sLightRotationRate.x * elapsedTime;
	}
	if (BRE::Keyboard::gInstance->IsKeyDown(DIK_RIGHTARROW)) {
		rotationAmount.x -= sLightRotationRate.x * elapsedTime;
	}
	if (BRE::Keyboard::gInstance->IsKeyDown(DIK_UPARROW)) {
		rotationAmount.y += sLightRotationRate.y * elapsedTime;
	}
	if (BRE::Keyboard::gInstance->IsKeyDown(DIK_DOWNARROW)) {
		rotationAmount.y -= sLightRotationRate.y * elapsedTime;
	}

	XMMATRIX lightRotationMatrix = XMMatrixIdentity();
	if (rotationAmount.x != 0) {
		lightRotationMatrix = XMMatrixRotationY(rotationAmount.x);
	}

	if (rotationAmount.y != 0) {
		const XMMATRIX lightRotationAxisMatrix = XMMatrixRotationAxis(mDirectionalLight.RightVector(), rotationAmount.y);
		lightRotationMatrix *= lightRotationAxisMatrix;
	}

	if (rotationAmount.x != 0.0f || rotationAmount.y != 0.0f) {
		mDirectionalLight.ApplyRotation(lightRotationMatrix);
	}
}