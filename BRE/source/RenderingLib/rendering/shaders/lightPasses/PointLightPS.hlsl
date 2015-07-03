#include <rendering/shaders/Lighting.hlsli>
#include <rendering/shaders/Utils.hlsli>

struct Input {
	float4 PosCS : SV_POSITION;
	nointerpolation float3 LightPosVS : POSITION;
	nointerpolation float4 LightColorAndRadius : LIGHT_COLOR_AND_RADIUS;
	float3 VertexPosVS : POS_VIEW_SPACE;
};

cbuffer CBufferPerFrame : register (b0) {
	float2 ProjectionFactors; // x -> Far clip distance / (Far clip distance / near clip distance)
							  // y -> (- Far clip distance * Near clip distance) / (Far clip distance - near clip distance)
}

SamplerState TexSampler : register (s0);

Texture2D NormalTexture : register (t0);
Texture2D BaseColorTexture : register (t1);
Texture2D Smoothness_MetalMask_Texture : register (t2);
Texture2D Reflectance_Texture : register (t3);
Texture2D DepthTexture : register (t4);

float4 main(const in Input input) : SV_TARGET{
	// Determine our indices for sampling the texture based on the current
	// screen position
	const int3 sampleIndices = int3(input.PosCS.xy, 0);
	const float depth = DepthTexture.Load(sampleIndices).x;
	const float linearDepth = ProjectionFactors.y / (depth - ProjectionFactors.x);
	const float3 viewRay = float3(input.VertexPosVS.xy / input.VertexPosVS.z, 1.0f);
	const float3 posVS = viewRay * linearDepth;
	
	const float3 normalVS = OctDecode(NormalTexture.Load(sampleIndices).xy);
	const float3 lightDir = input.LightPosVS - posVS;
	const float dist = length(lightDir);
	const float attenuation = max(0.0f, 1.0f - (dist / input.LightColorAndRadius.w));

	MaterialData data;
	data.BaseColor = BaseColorTexture.Load(sampleIndices).rgb;
	const float2 smoothness_metalMask = Smoothness_MetalMask_Texture.Load(sampleIndices).xy;
	data.MetalMask = smoothness_metalMask.y;
	data.Smoothness = smoothness_metalMask.x;
	data.Reflectance = Reflectance_Texture.Load(sampleIndices).rgb;
	const float3 final = attenuation * input.LightColorAndRadius.xyz * brdf(normalVS, normalize(-posVS), normalize(lightDir), data);
	return float4(final, 1.0f);
}



