#include <rendering/shaders/Utils.hlsli>
struct Input {
	float4 PosCS : SV_Position;
	float3 NormalVS : NORMAL;
};

struct Output {
	float3 NormalVS : SV_Target0;
	float3 BaseColor : SV_Target1;
	float3 Smoothness_MetalMask_Curvature : SV_Target2;
};

SamplerState TexSampler : register (s0);

Texture2D BaseColorTexture : register (t0);
Texture2D SmoothnessTexture : register (t1);
Texture2D MetalMaskTexture : register (t2);
Texture2D CurvatureTexture : register (t3);

Output main(Input input) {
	Output output = (Output)0;
	output.NormalVS = normalize(input.NormalVS);
	const float2 texCoord = float2(0.0f, 0.0f);
	output.BaseColor = BaseColorTexture.Sample(TexSampler, texCoord).rgb;
	output.Smoothness_MetalMask_Curvature.x = SmoothnessTexture.Sample(TexSampler, texCoord).x;
	output.Smoothness_MetalMask_Curvature.y = MetalMaskTexture.Sample(TexSampler, texCoord).x;
	output.Smoothness_MetalMask_Curvature.z = CurvatureTexture.Sample(TexSampler, texCoord).x;
	return output;
}