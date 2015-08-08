#include "Scene.h"

#include <general/Camera.h>
#include <input/Keyboard.h> 
#include <managers/DrawManager.h>
#include <managers/MaterialManager.h>   
#include <rendering/GlobalResources.h> 
#include <rendering/shaders/lightPasses/DirLightPsData.h>      
#include <utils/Assert.h>
#include <utils/MathUtils.h>   

using namespace DirectX;    

namespace { 
	const XMFLOAT2 sLightRotationRate(XM_PI / 4.0f, XM_PI / 4.0f); 

	const unsigned int sMaxShaderPointLights = 512;
	const unsigned int sNumPointLightShaders = 1; 

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
	 
	mDirectionalLight.SetColor(1.0f, 1.0f, 1.0f);
	BRE::DirectionalLightData& dirLightData = dirLightPsData.Light();        
	dirLightData.mColor = mDirectionalLight.Color();  
	dirLightData.mDirection = mDirectionalLight.Direction();      

	dirLightPsData.SamplerState() = BRE::GlobalResources::gInstance->MinMagMipPointSampler();   
}

void Scene::InitPointLights() {  
	std::vector<BRE::LightsDrawer::PointLightData>& quadCulledPointLightDataVec = BRE::DrawManager::gInstance->PointLightDataVec();
	BRE::LightsDrawer::PointLightData data;
	data.mPointLightVsData.SetNumLights(1);
	data.mPointLightVsData.LightPosAndRadius(0).x = -500.0f;
	data.mPointLightVsData.LightPosAndRadius(0).y = 200.0f;
	data.mPointLightVsData.LightPosAndRadius(0).z = 0.0f;
	data.mPointLightVsData.LightPosAndRadius(0).w = 1000.0f;
	const DirectX::XMFLOAT4 colorAndPower = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1000000.0f);
	data.mPointLightVsData.LightColorAndPower(0) = colorAndPower;
	quadCulledPointLightDataVec.push_back(data);

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