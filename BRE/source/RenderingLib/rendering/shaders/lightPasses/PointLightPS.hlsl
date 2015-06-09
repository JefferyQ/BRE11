#include <rendering/shaders/Lighting.hlsli>
#include <rendering/shaders/Utils.hlsli>

struct PsInput {
	float4 PosCS : SV_POSITION;
	nointerpolation float3 LightPosVS : POSITION;
	nointerpolation float4 LightColorAndRadius : LIGHT_COLOR_AND_RADIUS;
	float3 ViewRayVS : VIEW_RAY;
};

SamplerState TexSampler : register (s0);

Texture2D NormalTexture : register (t0);
Texture2D BaseColorTexture : register (t1);
Texture2D Smoothness_MetalMask_Texture : register (t2);
Texture2D Reflectance_Texture : register (t3);
Texture2D DepthTexture : register (t4);

float smoothDistanceAtt(float squaredDistance, float invSqrAttRadius) {
    float factor = squaredDistance * invSqrAttRadius;
	float smoothFactor = saturate(1.0f - factor * factor);
	return smoothFactor * smoothFactor;
}

 float getDistanceAtt(float3 unormalizedLightVector, float invSqrAttRadius) {
	 float sqrDist = dot(unormalizedLightVector, unormalizedLightVector);
	 float attenuation = 1.0f / (max(sqrDist, 0.01f * 0.01f));
	 attenuation *= smoothDistanceAtt(sqrDist, invSqrAttRadius);
	 return attenuation;
}

float4 main(const in PsInput IN) : SV_TARGET{
	// Determine our indices for sampling the texture based on the current
	// screen position
	const int3 sampleIndices = int3(IN.PosCS.xy, 0);
	const float depth = DepthTexture.Load(sampleIndices).x;
	const float3 posVS = IN.ViewRayVS * depth;
	const float3 normalVS = UnmapNormal(NormalTexture.Load(sampleIndices).xyz);
	const float3 lightDir = IN.LightPosVS - posVS;
	const float dist = length(lightDir);
	const float attenuation = max(0.0f, 1.0f - (dist / IN.LightColorAndRadius.w));

	// Process punctual light
	//const float lightInvSqrAttRadius = 1.0f / (IN.LightColorAndRadius.w * IN.LightColorAndRadius.w);
	//const float attenuation = getDistanceAtt(lightDir, lightInvSqrAttRadius);

	MaterialData data;
	data.BaseColor = BaseColorTexture.Load(sampleIndices).xyz;
	data.MetalMask = Smoothness_MetalMask_Texture.Load(sampleIndices).y;
	data.Smoothness = Smoothness_MetalMask_Texture.Load(sampleIndices).x;
	data.Reflectance = Reflectance_Texture.Load(sampleIndices).rgb;
	const float3 final = brdf(normalVS, normalize(-posVS), normalize(lightDir), data) * IN.LightColorAndRadius.xyz * attenuation;
	return float4(final, 1.0f);
}