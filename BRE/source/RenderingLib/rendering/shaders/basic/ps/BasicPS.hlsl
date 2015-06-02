#include <rendering/shaders/Utils.hlsli>

/*******************  Data  *************************/
struct VS_OUTPUT {
	float4 PosCS : SV_Position;
	float3 NormalVS : NORMAL;
	float DepthVS : DEPTH_VIEW_SPACE;
};

struct PS_OUTPUT {
	float3 NormalVS : SV_Target0;
	float3 BaseColor : SV_Target1;
	float Smoothness : SV_Target2;
	float MetalMask : SV_Target3;
	float Reflectance : SV_Target4;
	float DepthVS : SV_Target5;
};

cbuffer cbPerFrame : register (b0) {
	float FarClipPlaneDistance;
};

/*******************  Resources  *************************/
SamplerState TexSampler : register (s0);

Texture2D BaseColorTexture : register (t0);
Texture2D SmoothnessTexture : register (t1);
Texture2D MetalMaskTexture : register (t2);
Texture2D ReflectanceTexture : register (t3);

/*******************  Shader  *************************/
PS_OUTPUT main(VS_OUTPUT IN) {
	PS_OUTPUT OUT = (PS_OUTPUT)0;

	OUT.NormalVS = MapNormal(normalize(IN.NormalVS));
	OUT.BaseColor = BaseColorTexture.Sample(TexSampler, float2(0.0f, 0.0f)).rgb;
	OUT.Smoothness = SmoothnessTexture.Sample(TexSampler, float2(0.0f, 0.0f)).x;
	OUT.MetalMask = MetalMaskTexture.Sample(TexSampler, float2(0.0f, 0.0f)).x;
	OUT.Reflectance = ReflectanceTexture.Sample(TexSampler, float2(0.0f, 0.0f)).x;
	OUT.DepthVS = IN.DepthVS / FarClipPlaneDistance;

	return OUT;
}