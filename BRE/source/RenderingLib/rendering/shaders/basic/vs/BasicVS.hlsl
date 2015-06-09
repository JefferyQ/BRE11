struct VS_INPUT {
	float4 PosOS : POSITION;
	float3 NormalOS : NORMAL;
};

struct VS_OUTPUT {
	float4 PosCS : SV_Position;
	float3 NormalVS : NORMAL;
	float DepthVS : DEPTH_VIEW_SPACE;
};

cbuffer CBufferPerFrame : register (b0) {
	float4x4 WorldViewProj;
	float4x4 WorldView;
}

VS_OUTPUT main(const VS_INPUT IN) {
	VS_OUTPUT OUT = (VS_OUTPUT)0;
	OUT.PosCS = mul(IN.PosOS, WorldViewProj);
	OUT.DepthVS = mul(IN.PosOS, WorldView).z;
	OUT.NormalVS = normalize(mul(float4(IN.NormalOS, 0.0f), WorldView).xyz);
	return OUT;
}