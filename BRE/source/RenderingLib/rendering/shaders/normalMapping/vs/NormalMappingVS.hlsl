/*******************  Data  *************************/
struct VS_INPUT {
	float4 PosOS : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 NormalOS : NORMAL;
	float3 TangentOS : TANGENT;
};

struct VS_OUTPUT {
	float4 PosCS : SV_Position;
	float3 NormalWS : NORMAL;
	float DepthVS : DEPTH_VIEW_SPACE;
	float2 TexCoord : TEXCOORD0;
	float3 TangentWS : TANGENT;
	float3 BinormalWS : BINORMAL;
};

/*******************  Resources  *************************/
cbuffer CBufferPerFrame : register (b0) {
	float4x4 WorldViewProj;
	float4x4 WorldView;
	float4x4 World;
	float TextureScaleFactor;
}

/*******************  Shader  *************************/
VS_OUTPUT main(const VS_INPUT IN) {
	VS_OUTPUT OUT = (VS_OUTPUT)0;

	OUT.PosCS = mul(IN.PosOS, WorldViewProj);
	OUT.DepthVS = mul(IN.PosOS, WorldView).z;
	OUT.NormalWS = normalize(mul(float4(IN.NormalOS, 1.0f), World).xyz);
	OUT.TexCoord = IN.TexCoord * TextureScaleFactor;
	OUT.TangentWS = normalize(mul(float4(IN.TangentOS, 1.0f), World).xyz);
	OUT.BinormalWS = normalize(cross(OUT.NormalWS, OUT.TangentWS));

	return OUT;
}