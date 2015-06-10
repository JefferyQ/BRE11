#include <rendering/shaders/Utils.hlsli>

struct DS_OUTPUT {
	float4 PosCS : SV_Position;
	float3 NormalVS : NORMAL;
	float DepthVS : DEPTH_VIEW_SPACE;
	float2 TexCoord : TEXCOORD0;
	float3 TangentVS : TANGENT;
	float3 BinormalVS : BINORMAL;
};

struct PS_OUTPUT {
	float2 NormalVS : SV_Target0;
	float3 BaseColor : SV_Target1;
	float4 Smoothness_MetalMask : SV_Target2;
	float3 Reflectance : SV_Target3;
	float DepthVS : SV_Target4;
};

cbuffer cbPerFrame : register (b0) {
	float FarClipPlaneDistance;
};

SamplerState TexSampler : register (s0);

Texture2D NormalTexture : register (t0);
Texture2D BaseColorTexture : register (t1);
Texture2D SmoothnessTexture : register (t2);
Texture2D MetalMaskTexture : register (t3);
Texture2D ReflectanceTexture : register (t4);

PS_OUTPUT main(DS_OUTPUT IN) {
	PS_OUTPUT OUT = (PS_OUTPUT)0;
	const float3 sampledNormal = normalize(UnmapNormal(NormalTexture.Sample(TexSampler, IN.TexCoord).xyz));
	const float3x3 tbn = float3x3(normalize(IN.TangentVS), normalize(IN.BinormalVS), normalize(IN.NormalVS));
	OUT.NormalVS = OctEncode(mul(sampledNormal, tbn));
	OUT.BaseColor = BaseColorTexture.Sample(TexSampler, IN.TexCoord).rgb;
	OUT.Smoothness_MetalMask.x = SmoothnessTexture.Sample(TexSampler, IN.TexCoord).x;
	OUT.Smoothness_MetalMask.y = MetalMaskTexture.Sample(TexSampler, IN.TexCoord).x;
	OUT.Reflectance = ReflectanceTexture.Sample(TexSampler, IN.TexCoord).rgb;
	OUT.DepthVS = IN.DepthVS / FarClipPlaneDistance;
	return OUT;
}