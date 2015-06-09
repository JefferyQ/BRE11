#include "Scene.h"

#include <general/Camera.h>
#include <input/Keyboard.h>
#include <managers/DrawManager.h>
#include <managers/MaterialManager.h>
#include <rendering/GlobalResources.h>
#include <rendering/shaders/lightPasses/DirLightPsData.h> 
#include <utils/Assert.h>
#include <utils/Memory.h>
#include <utils/Utility.h>

using namespace DirectX;

namespace {
	const XMFLOAT2 sLightRotationRate(XM_PI / 4.0f, XM_PI / 4.0f); 

	const unsigned int sMaxShaderPointLights = 512;
	const unsigned int sNumPointLightShaders = 2; 

	const char* sMaterialsFile = "content\\configs\\materials.yml"; 
	const char* sSceneModelsFile = "content\\configs\\fullyDeferred\\models.yml";  
}

Scene::Scene() { 
	InitDirectionalLights();  
	InitPointLights();

	BRE::MaterialManager::gInstance->LoadMaterials(sMaterialsFile);     
	BRE::DrawManager::gInstance->LoadModels(sSceneModelsFile);  
}

void Scene::Update(const float elapsedTime) { 
	UpdateDirectionalLight(elapsedTime); 
	mPosUpdater.Update(elapsedTime);

	UpdatePointLights(elapsedTime);

	// Directional
	std::vector<BRE::LightsDrawer::DirLightData>& dirLightDataVec = BRE::DrawManager::gInstance->DirLightDataVec();
	const XMMATRIX viewMatrix = BRE::Camera::gInstance->ViewMatrix();
	const XMFLOAT4 lightDir = XMFLOAT4(mDirectionalLight.Direction().x, mDirectionalLight.Direction().y, mDirectionalLight.Direction().z, 0.0f);
	for (BRE::LightsDrawer::DirLightData& data : dirLightDataVec) {
		XMFLOAT4 s;
		XMStoreFloat4(&s, XMVector4Transform(XMLoadFloat4(&lightDir), viewMatrix));
		data.mPixelShaderData.Light().mDirection.x = s.x;
		data.mPixelShaderData.Light().mDirection.y = s.y;
		data.mPixelShaderData.Light().mDirection.z = s.z;
	}  
}

void Scene::InitDirectionalLights() {  
	std::vector<BRE::LightsDrawer::DirLightData>& dirLightDataVec = BRE::DrawManager::gInstance->DirLightDataVec();       
	dirLightDataVec.resize(1); 
	BRE::DirLightPixelShaderData& dirLightPsData = dirLightDataVec[0].mPixelShaderData;        
	 
	mDirectionalLight.SetColor(0.5f, 0.5f, 0.5f);
	BRE::DirectionalLightData& dirLightData = dirLightPsData.Light();
	dirLightData.mColor = mDirectionalLight.Color(); 
	dirLightData.mDirection = mDirectionalLight.Direction();

	dirLightPsData.SamplerState() = BRE::GlobalResources::gInstance->MinMagMipPointSampler(); 
}

void Scene::InitPointLights() {
	std::vector<BRE::LightsDrawer::PointLightData>& quadCulledPointLightDataVec = BRE::DrawManager::gInstance->PointLightDataVec();
	quadCulledPointLightDataVec.resize(sNumPointLightShaders);
	unsigned int lightIndex = 0;
	const float factor = 500.0f;
	for (BRE::LightsDrawer::PointLightData& data : quadCulledPointLightDataVec) {
		for (unsigned int iLight = 0; iLight < sMaxShaderPointLights; ++iLight, ++lightIndex) {
			data.mPointLightVsData.LightPosAndRadius(iLight).x = BRE::Utility::RandomFloat(-factor, factor);
			data.mPointLightVsData.LightPosAndRadius(iLight).y = BRE::Utility::RandomFloat(-factor, factor);
			data.mPointLightVsData.LightPosAndRadius(iLight).z = BRE::Utility::RandomFloat(-factor, factor);
			data.mPointLightVsData.LightPosAndRadius(iLight).w = 40; 
			const float c = BRE::Utility::RandomFloat(0.5f, 1.0f);
			DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(c, c, c, 0.0f);
			data.mPointLightVsData.LightColor(iLight) = color;

			const float f5 = BRE::Utility::RandomFloat(25.0f, 30.0f) * ((rand() % 2) ? 1.0f : -1.0f);  
			const float f6 = BRE::Utility::RandomFloat(25.0f, 30.0f) * ((rand() % 2) ? 1.0f : -1.0f); 
			const float f7 = BRE::Utility::RandomFloat(25.0f, 30.0f ) * ((rand() % 2) ? 1.0f : -1.0f);
			mPosUpdater.Add(BRE::PositionUpdater::Params(&data.mPointLightVsData.LightPosAndRadius(iLight), XMFLOAT3(-factor, -factor, -factor), XMFLOAT3(factor, factor, factor), XMFLOAT3(f5, f6, f7)));
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

void Scene::UpdatePointLights(const float elapsedTime) {
	const float radiusFactor = 30.0f;
	if (BRE::Keyboard::gInstance->IsKeyDown(DIK_I)) {
		std::vector<BRE::LightsDrawer::PointLightData>& quadCulledPointLightDataVec = BRE::DrawManager::gInstance->PointLightDataVec();
		unsigned int lightIndex = 0;
		for (BRE::LightsDrawer::PointLightData& data : quadCulledPointLightDataVec) {
			for (unsigned int iLight = 0; iLight < sMaxShaderPointLights; ++iLight, ++lightIndex) {
				data.mPointLightVsData.LightPosAndRadius(iLight).w += radiusFactor * elapsedTime;				
			}
		}
	}

	if (BRE::Keyboard::gInstance->IsKeyDown(DIK_K)) {
		std::vector<BRE::LightsDrawer::PointLightData>& quadCulledPointLightDataVec = BRE::DrawManager::gInstance->PointLightDataVec();
		unsigned int lightIndex = 0;
		for (BRE::LightsDrawer::PointLightData& data : quadCulledPointLightDataVec) {
			for (unsigned int iLight = 0; iLight < sMaxShaderPointLights; ++iLight, ++lightIndex) {
				data.mPointLightVsData.LightPosAndRadius(iLight).w -= radiusFactor * elapsedTime;
			}
		}
	}

	const float intensityFactor = 5.0f;
	if (BRE::Keyboard::gInstance->IsKeyDown(DIK_O)) {
		std::vector<BRE::LightsDrawer::PointLightData>& quadCulledPointLightDataVec = BRE::DrawManager::gInstance->PointLightDataVec();
		unsigned int lightIndex = 0;
		for (BRE::LightsDrawer::PointLightData& data : quadCulledPointLightDataVec) {
			for (unsigned int iLight = 0; iLight < sMaxShaderPointLights; ++iLight, ++lightIndex) {
				data.mPointLightVsData.LightColor(iLight).x += intensityFactor * elapsedTime;
				data.mPointLightVsData.LightColor(iLight).y += intensityFactor * elapsedTime;
				data.mPointLightVsData.LightColor(iLight).z += intensityFactor * elapsedTime;
			}
		}
	}

	if (BRE::Keyboard::gInstance->IsKeyDown(DIK_L)) {
		std::vector<BRE::LightsDrawer::PointLightData>& quadCulledPointLightDataVec = BRE::DrawManager::gInstance->PointLightDataVec();
		unsigned int lightIndex = 0;
		for (BRE::LightsDrawer::PointLightData& data : quadCulledPointLightDataVec) {
			for (unsigned int iLight = 0; iLight < sMaxShaderPointLights; ++iLight, ++lightIndex) {
				data.mPointLightVsData.LightColor(iLight).x -= intensityFactor * elapsedTime;
				data.mPointLightVsData.LightColor(iLight).y -= intensityFactor * elapsedTime;
				data.mPointLightVsData.LightColor(iLight).z -= intensityFactor * elapsedTime;
			}
		}
	}
}