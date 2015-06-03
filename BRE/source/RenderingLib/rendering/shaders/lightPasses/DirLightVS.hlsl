struct VS_INPUT {
	float4 PosCS : POSITION;
};

struct VS_OUTPUT {
	float4 PosCS : SV_Position;
	float3 ViewRayVS : VIEW_RAY;
};

cbuffer CBufferPerFrame : register (b0) {
	float4x4 InvProj;
};

VS_OUTPUT main(const VS_INPUT IN) {
	VS_OUTPUT OUT = (VS_OUTPUT)0;
	OUT.PosCS = IN.PosCS;
	OUT.ViewRayVS = mul(IN.PosCS, InvProj).xyz;
	return OUT;
}