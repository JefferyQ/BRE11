#include <rendering/shaders/Utils.hlsli>
struct VS_OUTPUT {
	float4 PosCS : SV_Position;
	float3 NormalVS : NORMAL;
	float DepthVS : DEPTH_VIEW_SPACE;
};

struct PS_OUTPUT {
	float3 NormalVS : SV_Target0;
	float3 BaseColor : SV_Target1;
	float2 Smoothness_MetalMask : SV_Target2;
	float3 Reflectance : SV_Target3;
	float DepthVS : SV_Target4;
};

cbuffer cbPerFrame : register (b0) {
	float FarClipPlaneDistance;
};

SamplerState TexSampler : register (s0);

Texture2D BaseColorTexture : register (t0);
Texture2D SmoothnessTexture : register (t1);
Texture2D MetalMaskTexture : register (t2);
Texture2D ReflectanceTexture : register (t3);

PS_OUTPUT main(VS_OUTPUT IN) {
	PS_OUTPUT OUT = (PS_OUTPUT)0;
	OUT.NormalVS = MapNormal(normalize(IN.NormalVS));
	const float2 texCoord = float2(0.0f, 0.0f);
	OUT.BaseColor = BaseColorTexture.Sample(TexSampler, texCoord).rgb;
	OUT.Smoothness_MetalMask.x = SmoothnessTexture.Sample(TexSampler, texCoord).x;
	OUT.Smoothness_MetalMask.y = MetalMaskTexture.Sample(TexSampler, texCoord).x;
	OUT.Reflectance = ReflectanceTexture.Sample(TexSampler, texCoord).rgb;
	OUT.DepthVS = IN.DepthVS / FarClipPlaneDistance;
	return OUT;
}