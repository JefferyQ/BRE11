#include <rendering/shaders/Lighting.hlsli>
#include <rendering/shaders/Utils.hlsli>

/************** Data Structures ******************/
struct PsInput {
	float4 mPositionH : SV_POSITION;
	nointerpolation float3 mLightPosW : POSITION;
	nointerpolation float4 mLightColorAndRadius : LIGHT_COLOR_AND_RADIUS;
};

cbuffer CBufferPerFrame : register (b0) {
	float3 CameraPositionW;
};

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
	normal = Decode(NormalTexture.Load(sampleIndices).xy);
	position = PositionTexture.Load(sampleIndices).xyz;
	diffuseAlbedo = DiffuseAlbedoTexture.Load(sampleIndices).xyz;
	const float4 spec = SpecularAlbedoTexture.Load(sampleIndices);
	specularAlbedo = spec.xyz;
	specularPower = spec.w * 255.0f;
}

/********************* Shader *****************************/
float4 main(const in PsInput IN) : SV_TARGET{
	float3 normal;
	float3 position;
	float3 diffuseAlbedo;
	float3 specularAlbedo;
	float specularPower;

	// Sample the G-Buffer properties from the textures
	GetGBufferAttributes(IN.mPositionH.xy, normal, position, diffuseAlbedo, specularAlbedo, specularPower);

	PointLightContributionData data = (PointLightContributionData)0;
	data.mDiffuseColor = diffuseAlbedo;
	data.mSpecularColor = float4(specularAlbedo, specularPower);
	data.mPosition = position;
	data.mNormal = normal;
	data.mViewDir = CameraPositionW - position;
	data.mLight.mColor = IN.mLightColorAndRadius.xyz;
	data.mLight.mRange = IN.mLightColorAndRadius.w;
	data.mLight.mPosition = IN.mLightPosW;

	float3 color = float3(1.0f, 1.0f, 1.0f);
	computePointLightContribution(data, color);

	return float4(color, 1.0f);
}