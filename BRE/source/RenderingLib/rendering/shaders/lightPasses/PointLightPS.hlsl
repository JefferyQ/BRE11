#include <rendering/shaders/Lighting.hlsli>
#include <rendering/shaders/Utils.hlsli>

/************** Data Structures ******************/
struct PsInput {
	float4 PosCS : SV_POSITION;
	nointerpolation float3 LightPosVS : POSITION;
	nointerpolation float4 LightColorAndRadius : LIGHT_COLOR_AND_RADIUS;
	float3 ViewRayVS : VIEW_RAY;
};

cbuffer CBufferPerFrame : register (b0) {
	float3 CameraPositionVS;
};

SamplerState TexSampler : register (s0);

Texture2D NormalTexture : register (t0);
Texture2D BaseColorTexture : register (t1);
Texture2D SmoothnessTexture : register (t2);
Texture2D MetalMaskTexture : register (t3);
Texture2D ReflectanceTexture : register (t4);
Texture2D DepthTexture : register (t5);

/*************** Functions *************************/
// Helper function for extracting G-Buffer attributes
void GetGBufferAttributes(in float2 screenPos, in float3 viewRayVS, out float3 normalVS, out float3 posVS, out float3 baseColor) {
	// Determine our indices for sampling the texture based on the current
	// screen position
	const int3 sampleIndices = int3(screenPos.xy, 0);
	normalVS = NormalTexture.Load(sampleIndices).xyz;

	const float depth = DepthTexture.Load(sampleIndices).x;
	posVS = CameraPositionVS + depth * viewRayVS;

	baseColor = BaseColorTexture.Load(sampleIndices).rgb;
}

/********************* Shader *****************************/
float4 main(const in PsInput IN) : SV_TARGET{
	float3 normalVS;
	float3 posVS;
	float3 baseColor;

	// Sample the G-Buffer properties from the textures
	GetGBufferAttributes(IN.PosCS.xy, IN.ViewRayVS, normalVS, posVS, baseColor);

	const float3 lightDir = IN.LightPosVS - posVS;
	const float dist = length(lightDir);
	const float attenuation = max(0.0f, 1.0f - (dist / IN.LightColorAndRadius.w));

	const int3 sampleIndices = int3(IN.PosCS.xy, 0);

	const float3 metallic = 1.0f - MetalMaskTexture.Load(sampleIndices).x;

	MaterialData data;
	data.DiffuseColor = baseColor - baseColor * metallic;
	data.SpecularColor = lerp(0.03f, baseColor, metallic);
	data.Roughness = pow(1.0f - SmoothnessTexture.Load(sampleIndices).r, 2.0f);
	data.ReflectanceAtNormalIncidence = ReflectanceTexture.Load(sampleIndices).x;
	data.ReflectanceAtNormalIncidence *= data.ReflectanceAtNormalIncidence * 0.16f;
	const float3 final = cook_torrance(normalVS, normalize(CameraPositionVS - posVS), normalize(lightDir), data) * IN.LightColorAndRadius.xyz * attenuation;
	return float4(final, 1.0f);
}