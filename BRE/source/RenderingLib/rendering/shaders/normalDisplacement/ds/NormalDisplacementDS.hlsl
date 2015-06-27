#define NUM_PATCH_POINTS 3

struct HS_CONSTANT_OUTPUT {
	float EdgeFactors[3] : SV_TessFactor;
	float InsideFactors : SV_InsideTessFactor;
};

struct HS_OUTPUT {
	float4 PosOS : POSITION;
	float3 NormalOS : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 TangentOS : TANGENT;
};

struct DS_OUTPUT {
	float4 PosCS : SV_Position;
	float3 NormalVS : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 TangentVS : TANGENT;
	float3 BinormalVS : BINORMAL;
};

cbuffer CBufferPerFrame : register (b0) {
	float4x4 Proj;
	float4x4 WorldView;
	float DisplacementScale;
}

SamplerState TexSampler : register (s0);

Texture2D DisplacementMap : register (t0);

[domain("tri")]
DS_OUTPUT main(const HS_CONSTANT_OUTPUT IN, const float3 uvw : SV_DomainLocation, const OutputPatch <HS_OUTPUT, NUM_PATCH_POINTS> patch) {
	DS_OUTPUT OUT = (DS_OUTPUT)0;

	OUT.TexCoord = uvw.x * patch[0].TexCoord + uvw.y * patch[1].TexCoord + uvw.z * patch[2].TexCoord;

	const float3 normalOS = uvw.x * patch[0].NormalOS + uvw.y * patch[1].NormalOS + uvw.z * patch[2].NormalOS;
	OUT.NormalVS = normalize(mul(float4(normalOS, 0.0f), WorldView).xyz);

	// Compute SV_Position by displacing object position in y coordinate
	float4 posVS = mul(uvw.x * patch[0].PosOS + uvw.y * patch[1].PosOS + uvw.z * patch[2].PosOS, WorldView);
	const float displacement = (DisplacementMap.SampleLevel(TexSampler, OUT.TexCoord, 0).x - 1.0f) * DisplacementScale;
	posVS += float4(OUT.NormalVS * displacement, 0.0f);
	OUT.PosCS = mul(posVS, Proj);

	// Compute world tangent and binormal
	OUT.TangentVS = normalize(uvw.x * patch[0].TangentOS + uvw.y * patch[1].TangentOS + uvw.z * patch[2].TangentOS);
	OUT.TangentVS = normalize(mul(float4(OUT.TangentVS, 0.0f), WorldView).xyz);
	OUT.BinormalVS = normalize(cross(OUT.NormalVS, OUT.TangentVS));

	return OUT;
}