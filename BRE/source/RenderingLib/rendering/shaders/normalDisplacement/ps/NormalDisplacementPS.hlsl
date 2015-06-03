#include <rendering/shaders/Utils.hlsli>

/*******************  Data  *************************/
struct DS_OUTPUT {
	float4 PosCS : SV_Position;
	float3 NormalVS : NORMAL;
	float DepthVS : DEPTH_VIEW_SPACE;
	float2 TexCoord : TEXCOORD0;
	float3 TangentVS : TANGENT;
	float3 BinormalVS : BINORMAL;
};

struct PS_OUTPUT {
	float3 NormalVS : SV_Target0;
	float3 BaseColor : SV_Target1;
	float4 Smoothness_MetalMask_Reflectance : SV_Target2;
	float DepthVS : SV_Target3;
};

cbuffer cbPerFrame : register (b0) {
	float FarClipPlaneDistance;
};

/*******************  Resources  *************************/
SamplerState TexSampler : register (s0);

Texture2D NormalTexture : register (t0);
Texture2D BaseColorTexture : register (t1);
Texture2D SmoothnessTexture : register (t2);
Texture2D MetalMaskTexture : register (t3);
Texture2D ReflectanceTexture : register (t4);

/*******************  Shader  *************************/
PS_OUTPUT main(DS_OUTPUT IN) {
	PS_OUTPUT OUT = (PS_OUTPUT)0;

	const float3 sampledNormal = normalize(UnmapNormal(NormalTexture.Sample(TexSampler, IN.TexCoord).xyz));
	const float3x3 tbn = float3x3(normalize(IN.TangentVS), normalize(IN.BinormalVS), normalize(IN.NormalVS));
	OUT.NormalVS = MapNormal(mul(sampledNormal, tbn));
	OUT.BaseColor = BaseColorTexture.Sample(TexSampler, IN.TexCoord).rgb;
	OUT.Smoothness_MetalMask_Reflectance.x = SmoothnessTexture.Sample(TexSampler, IN.TexCoord).x;
	OUT.Smoothness_MetalMask_Reflectance.y = MetalMaskTexture.Sample(TexSampler, IN.TexCoord).x;
	OUT.Smoothness_MetalMask_Reflectance.z = ReflectanceTexture.Sample(TexSampler, IN.TexCoord).x;
	OUT.DepthVS = IN.DepthVS / FarClipPlaneDistance;

	return OUT;
}