/*******************  Data  *************************/
struct VS_INPUT {
	float4 PositionL : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 NormalL : NORMAL;
	float3 TangentL : TANGENT;
};

struct VS_OUTPUT {
	float4 PositionH : SV_Position;	
	float3 PositionW : POSITION;
	float3 PositionV : POSITION_VIEW;
	float3 NormalW : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 TangentW : TANGENT;
	float3 BinormalW : BINORMAL;
};

/*******************  Resources  *************************/
cbuffer CBufferPerFrame : register (b0) {
	float4x4 WorldViewProjection;
	float4x4 WorldView;
	float4x4 World;
}

/*******************  Shader  *************************/
VS_OUTPUT main(const VS_INPUT IN) {
	VS_OUTPUT OUT = (VS_OUTPUT)0;

	OUT.PositionH = mul(IN.PositionL, WorldViewProjection);
	OUT.PositionW = mul(IN.PositionL, World).xyz;
	OUT.PositionV = mul(IN.PositionL, WorldView).xyz;
	OUT.NormalW = normalize(mul(float4(IN.NormalL, 1.0f), World).xyz);
	OUT.TexCoord = IN.TexCoord;
	OUT.TangentW = normalize(mul(float4(IN.TangentL, 1.0f), World).xyz);
	OUT.BinormalW = normalize(cross(OUT.NormalW, OUT.TangentW));

	return OUT;
}