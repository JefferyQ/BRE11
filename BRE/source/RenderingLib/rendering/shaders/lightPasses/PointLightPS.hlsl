#include <rendering/shaders/Lighting.hlsli>
#include <rendering/shaders/Utils.hlsli>

/************** Data Structures ******************/
struct PsInput {
	float4 PosCS : SV_POSITION;
	nointerpolation float3 LightPosVS : POSITION;
	nointerpolation float4 LightColorAndRadius : LIGHT_COLOR_AND_RADIUS;
	float3 ViewRayVS : VIEW_RAY;
};

SamplerState TexSampler : register (s0);

Texture2D NormalTexture : register (t0);
Texture2D BaseColorTexture : register (t1);
Texture2D Smoothness_MetalMask_Reflectance_Texture : register (t2);
Texture2D DepthTexture : register (t3);

/********************* Shader *****************************/
float4 main(const in PsInput IN) : SV_TARGET{
	// Determine our indices for sampling the texture based on the current
	// screen position
	const int3 sampleIndices = int3(IN.PosCS.xy, 0);
	
	const float depth = DepthTexture.Load(sampleIndices).x;
	const float3 posVS = IN.ViewRayVS * depth;
	const float3 normalVS = UnmapNormal(NormalTexture.Load(sampleIndices).xyz);	
	const float3 baseColor = BaseColorTexture.Load(sampleIndices).rgb;
	const float3 lightDir = IN.LightPosVS - posVS;
	const float dist = length(lightDir);
	const float attenuation = max(0.0f, 1.0f - (dist / IN.LightColorAndRadius.w));	
	const float metalMask = Smoothness_MetalMask_Reflectance_Texture.Load(sampleIndices).y;

	MaterialData data;
	data.BaseColor = baseColor * (1.0f - metalMask);
	data.SpecularColor = lerp(0.03f, baseColor, metalMask);
	data.Roughness = 1.0f - Smoothness_MetalMask_Reflectance_Texture.Load(sampleIndices).x;
	const float f0 = Smoothness_MetalMask_Reflectance_Texture.Load(sampleIndices).z;
	data.ReflectanceAtNormalIncidence = 0.16f * f0 * f0;
	const float3 final = brdf(normalVS, normalize(-posVS), normalize(lightDir), data) * IN.LightColorAndRadius.xyz * attenuation;
	return float4(final, 1.0f);
}