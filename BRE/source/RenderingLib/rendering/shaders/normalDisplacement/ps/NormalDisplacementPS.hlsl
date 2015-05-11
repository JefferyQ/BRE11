#include <rendering/shaders/Utils.hlsli>

/*******************  Data  *************************/
struct DS_OUTPUT {
	float4 PositionH : SV_Position;
	float3 PositionW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 TangentW : TANGENT;
	float3 BinormalW : BINORMAL;
};

struct PS_OUTPUT {
	float2 Normal : SV_Target0;
	float4 DiffuseAlbedo : SV_Target1;
	float4 SpecularAlbedo : SV_Target2;
	float4 Position : SV_Target3;
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
	const float3x3 tbn = float3x3(normalize(IN.TangentW), normalize(IN.BinormalW), normalize(IN.NormalW));
	sampledNormal = normalize(mul(sampledNormal, tbn));
	const float4 diffuseColor = DiffuseTexture.Sample(TexSampler, IN.TexCoord);

	OUT.Normal = Encode(sampledNormal);
	OUT.DiffuseAlbedo = diffuseColor;
	OUT.SpecularAlbedo = SpecularMapTexture.Sample(TexSampler, IN.TexCoord);
	OUT.Position = float4(IN.PositionW, 1.0f);

	return OUT;
}