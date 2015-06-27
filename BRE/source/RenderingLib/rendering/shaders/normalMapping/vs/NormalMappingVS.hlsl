struct VS_INPUT {
	float4 PosOS : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 NormalOS : NORMAL;
	float3 TangentOS : TANGENT;
};

struct VS_OUTPUT {
	float4 PosCS : SV_Position;
	float3 NormalVS : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 TangentVS : TANGENT;
	float3 BinormalVS : BINORMAL;
};

cbuffer CBufferPerFrame : register (b0) {
	float4x4 WorldViewProj;
	float4x4 WorldView;
	float TextureScaleFactor;
}

VS_OUTPUT main(const VS_INPUT IN) {
	VS_OUTPUT OUT = (VS_OUTPUT)0;
	OUT.PosCS = mul(IN.PosOS, WorldViewProj);
	OUT.NormalVS = normalize(mul(float4(IN.NormalOS, 0.0f), WorldView).xyz);
	OUT.TexCoord = IN.TexCoord * TextureScaleFactor;
	OUT.TangentVS = normalize(mul(float4(IN.TangentOS, 0.0f), WorldView).xyz);
	OUT.BinormalVS = normalize(cross(OUT.NormalVS, OUT.TangentVS));
	return OUT;
}