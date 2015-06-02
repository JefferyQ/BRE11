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
}

SamplerState TexSampler : register (s0);

Texture2D NormalTexture : register (t0);
Texture2D BaseColorTexture : register (t1);
Texture2D SmoothnessTexture : register (t2); 
Texture2D MetalMaskTexture : register (t3);
Texture2D ReflectanceTexture : register (t4);
Texture2D DepthTexture : register (t5);

/********************* Shader *****************************/
float4 main(const in VS_OUTPUT IN) : SV_TARGET {
	// Determine our indices for sampling the texture based on the current
	// screen position
	const int3 sampleIndices = int3(IN.PosCS.xy, 0);
	const float depth = DepthTexture.Load(sampleIndices).x;
	const float3 posVS = IN.ViewRayVS * depth;
	const float3 normalVS = UnmapNormal(NormalTexture.Load(sampleIndices).xyz);
	const float3 baseColor = BaseColorTexture.Load(sampleIndices).xyz;
	const float metalMask = MetalMaskTexture.Load(sampleIndices).x;

	MaterialData data;	
	data.BaseColor = baseColor * (1.0f - metalMask);
	data.SpecularColor = lerp(0.03f, baseColor, metalMask);
	data.Roughness = 1.0f - SmoothnessTexture.Load(sampleIndices).r;
	const float f0 = ReflectanceTexture.Load(sampleIndices).x;
	data.ReflectanceAtNormalIncidence = 0.16f * f0 * f0 * metalMask;
	const float3 final = brdf(normalVS, normalize(-posVS), -normalize(Light.Direction), data) * Light.Color;
	return float4(final, 1.0f);
}