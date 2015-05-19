#include <rendering/shaders/Lighting.hlsli>
#include <rendering/shaders/Utils.hlsli>

/************** Data Structures ******************/
struct VS_OUTPUT {
	float4 PosCS : SV_Position;
	float3 ViewRayVS : VIEW_RAY;
};

/****************** Resources ***********************/
cbuffer CBufferPerFrame : register (b0) {
	DirectionalLight Light;
	float3 CameraPositionVS;
}

SamplerState TexSampler : register (s0);

Texture2D NormalTexture : register (t0);
Texture2D DiffuseAlbedoTexture : register (t1);
Texture2D SpecularAlbedoTexture : register (t2);
Texture2D DepthTexture : register (t3);

/*************** Functions *************************/
// Helper function for extracting G-Buffer attributes
void GetGBufferAttributes(in float2 screenPos, in float3 viewRayVS, out float3 normalVS, out float3 posVS, out float3 diffuseAlbedo, out float3 specularAlbedo, out float specularPower) {
	// Determine our indices for sampling the texture based on the current
	// screen position
	const int3 sampleIndices = int3(screenPos.xy, 0);
	normalVS = Decode(NormalTexture.Load(sampleIndices).xy);

	const float depth = DepthTexture.Load(sampleIndices).x;
	posVS = CameraPositionVS + depth * viewRayVS;

	diffuseAlbedo = DiffuseAlbedoTexture.Load(sampleIndices).xyz;
	const float4 spec = SpecularAlbedoTexture.Load(sampleIndices);
	specularAlbedo = spec.xyz;
	specularPower = spec.w * 255.0f;
}

/********************* Shader *****************************/
float4 main(const in VS_OUTPUT IN) : SV_TARGET{
	float3 normalVS;
	float3 posVS;
	float3 diffuseAlbedo;
	float3 specularAlbedo;
	float specularPower;

	// Sample the G-Buffer properties from the textures
	GetGBufferAttributes(IN.PosCS.xy, IN.ViewRayVS, normalVS, posVS, diffuseAlbedo, specularAlbedo, specularPower);

	DirLightContributionData data = (DirLightContributionData)0;
	data.DiffuseColor = diffuseAlbedo;
	data.SpecularColor = float4(specularAlbedo, specularPower);
	data.Normal = normalVS;
	data.Light = Light;
	data.ViewDir = CameraPositionVS - posVS;

	float3 color = float3(0.0f, 0.0f, 0.0f);
	computeDirLightContribution(data, color);

	return float4(color, 1.0f);
}