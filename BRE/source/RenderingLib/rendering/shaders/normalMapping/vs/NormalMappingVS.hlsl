/*******************  Data  *************************/
struct VS_INPUT {
	float4 PosL : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 NormalL : NORMAL;
	float3 TangentL : TANGENT;
};

struct VS_OUTPUT {
	float4 PosH : SV_Position;	
	float3 PosV : POSITION_VIEW;
	float3 NormalW : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 TangentW : TANGENT;
	float3 BinormalW : BINORMAL;
};

/*******************  Resources  *************************/
cbuffer CBufferPerFrame : register (b0) {
	float4x4 WorldViewProj;
	float4x4 WorldView;
	float4x4 World;
}

/*******************  Shader  *************************/
VS_OUTPUT main(const VS_INPUT IN) {
	VS_OUTPUT OUT = (VS_OUTPUT)0;

	OUT.PosH = mul(IN.PosL, WorldViewProj);
	OUT.PosV = mul(IN.PosL, WorldView).xyz;
	OUT.NormalW = normalize(mul(float4(IN.NormalL, 1.0f), World).xyz);
	OUT.TexCoord = IN.TexCoord;
	OUT.TangentW = normalize(mul(float4(IN.TangentL, 1.0f), World).xyz);
	OUT.BinormalW = normalize(cross(OUT.NormalW, OUT.TangentW));

	return OUT;
}