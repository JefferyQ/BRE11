struct VS_INPUT {
	float4 PosCS : POSITION;
};

struct VS_OUTPUT {
	float4 PosCS : SV_Position;
	float3 ViewRay : VIEW_RAY;
};

cbuffer CBufferPerFrame : register (b0) {
	float4x4 InvProj;
};

VS_OUTPUT main(const VS_INPUT IN) {
	VS_OUTPUT OUT = (VS_OUTPUT)0;
	OUT.PosCS = IN.PosCS;

	// Clamp the view space position to the plane at Z = 1
	const float3 posVS = mul(IN.PosCS, InvProj).xyz;
	OUT.ViewRay = float3(posVS.xy / posVS.z, 1.0f);
	return OUT;
}