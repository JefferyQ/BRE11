#include <rendering/shaders/Lighting.hlsli>
#include <rendering/shaders/Utils.hlsli>

/************** Data Structures ******************/
struct VS_OUTPUT {
	float4 PositionH : SV_Position;
};

/****************** Resources ***********************/
cbuffer CBufferPerFrame : register (b0) {
	DirectionalLight Light;
	float3 CameraPositionW;
}

SamplerState TexSampler : register (s0);

Texture2D NormalTexture : register (t0);
Texture2D DiffuseAlbedoTexture : register (t1);
Texture2D SpecularAlbedoTexture : register (t2);
Texture2D PositionTexture : register (t3);

/*************** Functions *************************/
// Helper function for extracting G-Buffer attributes
void GetGBufferAttributes(in float2 screenPos, out float3 normal, out float3 position, out float3 diffuseAlbedo, out float3 specularAlbedo, out float specularPower) {
	// Determine our indices for sampling the texture based on the current
	// screen position
	const int3 sampleIndices = int3(screenPos.xy, 0);
	const float4 spec = SpecularAlbedoTexture.Load(sampleIndices);
	normal = Decode(NormalTexture.Load(sampleIndices).xy);
	position = PositionTexture.Load(sampleIndices).xyz;
	diffuseAlbedo = DiffuseAlbedoTexture.Load(sampleIndices).xyz;

	specularAlbedo = spec.xyz;
	specularPower = spec.w * 255.0f;
}

/********************* Shader *****************************/
float4 main(const in VS_OUTPUT IN) : SV_TARGET{
	float3 normal;
	float3 position;
	float3 diffuseAlbedo;
	float3 specularAlbedo;
	float specularPower;

	// Sample the G-Buffer properties from the textures
	GetGBufferAttributes(IN.PositionH.xy, normal, position, diffuseAlbedo, specularAlbedo, specularPower);

	DirLightContributionData data = (DirLightContributionData)0;
	data.mDiffuseColor = diffuseAlbedo;
	data.mSpecularColor = float4(specularAlbedo, specularPower);
	data.mNormal = normal;
	data.mLight = Light;
	data.mViewDir = CameraPositionW - position;

	float3 color = float3(0.0f, 0.0f, 0.0f);
	computeDirLightContribution(data, color);

	return float4(color, 1.0f);
}