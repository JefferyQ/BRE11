#include <rendering/shaders/Utils.hlsli>

struct Input {
	float4 PosCS : SV_Position;
	float3 NormalVS : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 TangentVS : TANGENT;
	float3 BinormalVS : BINORMAL;
};

struct Output {
	float2 NormalVS : SV_Target0;
	float3 BaseColor : SV_Target1;
	float3 Smoothness_MetalMask_Curvature : SV_Target2;
};

SamplerState TexSampler : register (s0);

Texture2D NormalTexture : register (t0);
Texture2D BaseColorTexture : register (t1);
Texture2D SmoothnessTexture : register (t2);
Texture2D MetalMaskTexture : register (t3);
Texture2D CurvatureTexture : register (t4);

Output main(Input input) {
	Output output = (Output)0;
	const float3 sampledNormal = normalize(UnmapNormal(NormalTexture.Sample(TexSampler, input.TexCoord).xyz));
	const float3x3 tbn = float3x3(normalize(input.TangentVS), normalize(input.BinormalVS), normalize(input.NormalVS));
	output.NormalVS = OctEncode(mul(sampledNormal, tbn));
	output.BaseColor = BaseColorTexture.Sample(TexSampler, input.TexCoord).rgb;
	output.Smoothness_MetalMask_Curvature.x = SmoothnessTexture.Sample(TexSampler, input.TexCoord).x;
	output.Smoothness_MetalMask_Curvature.y = MetalMaskTexture.Sample(TexSampler, input.TexCoord).x;
	output.Smoothness_MetalMask_Curvature.z = CurvatureTexture.Sample(TexSampler, input.TexCoord).x;
	return output;
}