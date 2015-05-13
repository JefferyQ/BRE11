#include <rendering/shaders/Utils.hlsli>
#define FAR_CLIP_PLANE_DISTANCE 5000.0f

/*******************  Data  *************************/
struct VS_OUTPUT {
	float4 PosCS : SV_Position;
	float3 NormalWS : NORMAL;
	float DepthVS : DEPTH_VIEW_SPACE;
	float2 TexCoord : TEXCOORD0;
	float3 TangentWS : TANGENT;
	float3 BinormalWS : BINORMAL;
};

struct PS_OUTPUT {
	float2 NormalWS : SV_Target0;
	float4 DiffuseAlbedo : SV_Target1;
	float4 SpecularAlbedo : SV_Target2;
	float Depth : SV_Target3;
};

/*******************  Resources  *************************/
SamplerState TexSampler : register (s0);

Texture2D DiffuseTexture : register (t0);
Texture2D NormalMapTexture : register (t1);
Texture2D SpecularMapTexture : register (t2);

/*******************  Shader  *************************/
PS_OUTPUT main(VS_OUTPUT IN) {
	PS_OUTPUT OUT = (PS_OUTPUT)0;

	// Transform sampled normal from texture space to world space:
	// Map normal from [0..1] to [-1..1]
	float3 sampledNormal = normalize((2 * NormalMapTexture.Sample(TexSampler, IN.TexCoord).xyz) - 1.0);
	const float3x3 tbn = float3x3(normalize(IN.TangentWS), normalize(IN.BinormalWS), normalize(IN.NormalWS));
	sampledNormal = normalize(mul(sampledNormal, tbn));
	const float4 diffuseColor = DiffuseTexture.Sample(TexSampler, IN.TexCoord);

	OUT.NormalWS = Encode(sampledNormal);
	OUT.DiffuseAlbedo = diffuseColor;
	OUT.SpecularAlbedo = SpecularMapTexture.Sample(TexSampler, IN.TexCoord);
	OUT.Depth = IN.DepthVS / FAR_CLIP_PLANE_DISTANCE;

	return OUT;
}