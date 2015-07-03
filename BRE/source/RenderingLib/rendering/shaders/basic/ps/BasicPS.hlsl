#include <rendering/shaders/Utils.hlsli>
struct Input {
	float4 PosCS : SV_Position;
	float3 NormalVS : NORMAL;
};

struct Output {
	float2 NormalVS : SV_Target0;
	float3 BaseColor : SV_Target1;
	float2 Smoothness_MetalMask : SV_Target2;
	float3 Reflectance : SV_Target3;
};

SamplerState TexSampler : register (s0);

Texture2D BaseColorTexture : register (t0);
Texture2D SmoothnessTexture : register (t1);
Texture2D MetalMaskTexture : register (t2);
Texture2D ReflectanceTexture : register (t3);

Output main(Input input) {
	Output output = (Output)0;
	output.NormalVS = OctEncode(normalize(input.NormalVS));
	const float2 texCoord = float2(0.0f, 0.0f);
	output.BaseColor = BaseColorTexture.Sample(TexSampler, texCoord).rgb;
	output.Smoothness_MetalMask.x = SmoothnessTexture.Sample(TexSampler, texCoord).x;
	output.Smoothness_MetalMask.y = MetalMaskTexture.Sample(TexSampler, texCoord).x;
	output.Reflectance = ReflectanceTexture.Sample(TexSampler, texCoord).rgb;
	return output;
}