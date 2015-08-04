#include <rendering/shaders/Lighting.hlsli>
#include <rendering/shaders/Utils.hlsli>

struct Input {
	float4 PosCS : SV_POSITION;
	nointerpolation float4 LightPosVSAndRadius : POSITION_AND_RADIUS;
	nointerpolation float4 LightColorAndPower : LIGHT_COLOR_AND_POWER;
	float3 VertexPosVS : POS_VIEW_SPACE;
};

cbuffer CBufferPerFrame : register (b0) {
	float2 ProjectionFactors; // x -> Far clip distance / (Far clip distance - near clip distance)
							  // y -> (- Far clip distance * Near clip distance) / (Far clip distance - near clip distance)
}

SamplerState TexSampler : register (s0);

Texture2D NormalTexture : register (t0);
Texture2D BaseColorTexture : register (t1);
Texture2D Smoothness_MetalMask_Curvature_Texture : register (t2);
Texture2D DepthTexture : register (t3);

float smoothDistanceAtt(const float squaredDistance, const float invSqrAttRadius) {
	const float factor = squaredDistance * invSqrAttRadius;
	const float smoothFactor = saturate(1.0f - factor * factor);
	return smoothFactor * smoothFactor;
}

float getDistanceAtt(float3 unormalizedLightVector, float invSqrAttRadius) {
	const float sqrDist = dot(unormalizedLightVector, unormalizedLightVector);
	float attenuation = 1.0f / (max(sqrDist, 0.01f * 0.01f));
	attenuation *= smoothDistanceAtt(sqrDist, invSqrAttRadius);
	return attenuation;
}

float getAngleAtt(const float3 normalizedLightVector, const float3 lightDir, const float lightAngleScale, const float lightAngleOffset) {
    // On the CPU
	// float lightAngleScale = 1.0 f / max (0.001f, ( cosInner - cosOuter ));
	// float lightAngleOffset = -cosOuter * angleScale ;
	const float cd = dot(lightDir, normalizedLightVector);
	float attenuation = saturate(cd * lightAngleScale + lightAngleOffset);
	// smooth the transition
	attenuation *= attenuation;
	return attenuation;
}

float4 main(const in Input input) : SV_TARGET{
	// Determine our indices for sampling the texture based on the current
	// screen position
	const int3 sampleIndices = int3(input.PosCS.xy, 0);
	const float depth = DepthTexture.Load(sampleIndices).x;
	const float linearDepth = ProjectionFactors.y / (depth - ProjectionFactors.x);
	const float3 viewRay = float3(input.VertexPosVS.xy / input.VertexPosVS.z, 1.0f);
	const float3 posVS = viewRay * linearDepth;
	
	const float3 normalVS = NormalTexture.Load(sampleIndices).xyz;
	const float3 lightDir = input.LightPosVSAndRadius.xyz - posVS;

	// Process punctual light
	const float3 unnormalizedLightVector = input.LightPosVSAndRadius.xyz - posVS;
	const float lightInvSqrAttRadius = 1.0f / (input.LightPosVSAndRadius.w * input.LightPosVSAndRadius.w);
	const float att = getDistanceAtt(unnormalizedLightVector, lightInvSqrAttRadius);
	const float3 lightColor = input.LightColorAndPower.w * input.LightColorAndPower.xyz / (4.0f * 3.141592f);

	MaterialData data;
	data.BaseColor = BaseColorTexture.Load(sampleIndices).rgb;
	const float3 smoothness_metalMask_curvature = Smoothness_MetalMask_Curvature_Texture.Load(sampleIndices).xyz;
	data.Smoothness = smoothness_metalMask_curvature.x;
	data.MetalMask = smoothness_metalMask_curvature.y;
	data.Curvature = smoothness_metalMask_curvature.z;
	const float3 final = att * lightColor * brdf(normalVS, normalize(-posVS), normalize(lightDir), data);
	return float4(final, 1.0f);
}



