#include <rendering/shaders/Utils.hlsli>

/*******************  Data  *************************/
struct DS_OUTPUT {
	float4 PosCS : SV_Position;
	float3 NormalWS : NORMAL;
	float DepthVS : DEPTH_VIEW_SPACE;
	float2 TexCoord : TEXCOORD0;
	float3 TangentWS : TANGENT;
	float3 BinormalWS : BINORMAL;
};

struct PS_OUTPUT {
	float3 NormalVS : SV_Target0;
	float4 DiffuseAlbedo : SV_Target1;
	float4 SpecularAlbedo : SV_Target2;
	float DepthVS : SV_Target3;
};

cbuffer cbPerFrame : register (b0) {
	float4x4 View;
	float FarClipPlaneDistance;
};

/*******************  Resources  *************************/
SamplerState TexSampler : register (s0);

Texture2D DiffuseTexture : register (t0);
Texture2D NormalMapTexture : register (t1);
Texture2D SpecularMapTexture : register (t2);

/*******************  Shader  *************************/
PS_OUTPUT main(DS_OUTPUT IN) {
	PS_OUTPUT OUT = (PS_OUTPUT)0;

	// Transform sampled normal from texture space to world space:
	// Map normal from [0..1] to [-1..1]
	float3 sampledNormal = normalize((2 * NormalMapTexture.Sample(TexSampler, IN.TexCoord).xyz) - 1.0);
	const float3x3 tbn = float3x3(normalize(IN.TangentWS), normalize(IN.BinormalWS), normalize(IN.NormalWS));
	sampledNormal = mul(sampledNormal, tbn);
	sampledNormal = normalize(mul(float4(sampledNormal, 0.0f), View).xyz);
	OUT.NormalVS = sampledNormal;
	OUT.DiffuseAlbedo = DiffuseTexture.Sample(TexSampler, IN.TexCoord);
	OUT.SpecularAlbedo = SpecularMapTexture.Sample(TexSampler, IN.TexCoord);
	OUT.DepthVS = IN.DepthVS / FarClipPlaneDistance;

	return OUT;
}