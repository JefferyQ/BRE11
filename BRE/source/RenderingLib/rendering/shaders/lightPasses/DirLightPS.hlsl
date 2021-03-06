#include <rendering/shaders/Lighting.hlsli>
#include <rendering/shaders/Utils.hlsli>

struct Input {
	float4 PosCS : SV_Position;
	float3 ViewRay : VIEW_RAY;
};

cbuffer CBufferPerFrame : register (b0) {
	DirectionalLight Light;
	float ProjectionA; // Far clip distance / (Far clip distance - near clip distance)
	float ProjectionB; // (- Far clip distance * Near clip distance) / (Far clip distance - near clip distance)
}

SamplerState TexSampler : register (s0);

Texture2D NormalTexture : register (t0);
Texture2D BaseColorTexture : register (t1);
Texture2D Smoothness_MetalMask_Curvature_Texture : register (t2);
Texture2D DepthTexture : register (t3);

float4 main(const in Input input) : SV_TARGET { 
	// Determine our indices for sampling the texture based on the current screen position
	const int3 sampleIndices = int3(input.PosCS.xy, 0);
	const float depth = DepthTexture.Load(sampleIndices).x;
	const float linearDepth = ProjectionB / (depth - ProjectionA);
	const float3 posVS = input.ViewRay * linearDepth;
	const float3 normalVS = NormalTexture.Load(sampleIndices).xyz;
	 
	MaterialData data;
	data.BaseColor = accurateSRGBToLinear(BaseColorTexture.Load(sampleIndices).xyz);
	const float3 smoothness_metalMask_curvature = Smoothness_MetalMask_Curvature_Texture.Load(sampleIndices).xyz;
	data.Smoothness = smoothness_metalMask_curvature.x;
	data.MetalMask = smoothness_metalMask_curvature.y;
	data.Curvature = smoothness_metalMask_curvature.z;
	const float3 final = data.BaseColor * 0.01f + brdf(normalVS, normalize(-posVS), -normalize(Light.Direction), data) * Light.Color;
	return float4(final, 1.0f);
}