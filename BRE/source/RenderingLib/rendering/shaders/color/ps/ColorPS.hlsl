#include <rendering/shaders/Utils.hlsli>

/*******************  Data  *************************/
struct VS_OUTPUT {
	float4 PosCS : SV_Position;
	float4 Color : COLOR;
	float3 NormalWS : NORMAL;
	float DepthVS : DEPTH_VIEW_SPACE;
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

/*******************  Shader  *************************/
PS_OUTPUT main(VS_OUTPUT IN) {
	PS_OUTPUT OUT = (PS_OUTPUT)0;

	OUT.NormalVS = normalize(mul(float4(normalize(IN.NormalWS), 0.0f), View).xyz);
	OUT.DiffuseAlbedo = IN.Color;
	OUT.SpecularAlbedo = float4(1.0f, 1.0f, 1.0f, 1.0f);
	OUT.DepthVS = IN.DepthVS / FarClipPlaneDistance;

	return OUT;
}