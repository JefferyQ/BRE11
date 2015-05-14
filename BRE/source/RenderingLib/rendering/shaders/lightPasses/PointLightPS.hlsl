#include <rendering/shaders/Lighting.hlsli>
#include <rendering/shaders/Utils.hlsli>

/************** Data Structures ******************/
struct PsInput {
	float4 PosCS : SV_POSITION;
	nointerpolation float3 LightPosWS : POSITION;
	nointerpolation float4 LightColorAndRadius : LIGHT_COLOR_AND_RADIUS;
	float3 ViewRayVS : VIEW_RAY;
};

cbuffer CBufferPerFrame : register (b0) {
	float4x4 InvView;
	float3 CameraPositionWS;
	float3 CameraDirectionWS;
};

SamplerState TexSampler : register (s0);

Texture2D NormalTexture : register (t0);
Texture2D DiffuseAlbedoTexture : register (t1);
Texture2D SpecularAlbedoTexture : register (t2);
Texture2D DepthTexture : register (t3);

/*************** Functions *************************/
// Helper function for extracting G-Buffer attributes
void GetGBufferAttributes(in float2 screenPos, in float3 viewRayVS, out float3 normalWS, out float3 posWS, out float3 diffuseAlbedo, out float3 specularAlbedo, out float specularPower) {
	// Determine our indices for sampling the texture based on the current
	// screen position
	const int3 sampleIndices = int3(screenPos.xy, 0);
	normalWS = Decode(NormalTexture.Load(sampleIndices).xy);

	const float depth = DepthTexture.Load(sampleIndices).x;
	posWS = CameraPositionWS + mul(float4(depth * viewRayVS, 0.0f), InvView).xyz;

	diffuseAlbedo = DiffuseAlbedoTexture.Load(sampleIndices).xyz;
	const float4 spec = SpecularAlbedoTexture.Load(sampleIndices);
	specularAlbedo = spec.xyz;
	specularPower = spec.w * 255.0f;
}

/********************* Shader *****************************/
float4 main(const in PsInput IN) : SV_TARGET{
	float3 normalWS;
	float3 posWS;
	float3 diffuseAlbedo;
	float3 specularAlbedo;
	float specularPower;

	// Sample the G-Buffer properties from the textures
	GetGBufferAttributes(IN.PosCS.xy, IN.ViewRayVS, normalWS, posWS, diffuseAlbedo, specularAlbedo, specularPower);

	PointLightContributionData data = (PointLightContributionData)0;
	data.DiffuseColor = diffuseAlbedo;
	data.SpecularColor = float4(specularAlbedo, specularPower);
	data.PosWS = posWS;
	data.NormalWS = normalWS;
	data.ViewDirWS = CameraPositionWS - posWS;
	data.Light.Color = IN.LightColorAndRadius.xyz;
	data.Light.Range = IN.LightColorAndRadius.w;
	data.Light.PosWS = IN.LightPosWS;

	float3 color = float3(0.0f, 0.0f, 0.0f);
	computePointLightContribution(data, color);

	return float4(color, 1.0f);
}