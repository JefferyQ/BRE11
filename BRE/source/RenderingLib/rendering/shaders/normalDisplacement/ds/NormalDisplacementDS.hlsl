/*******************  Data  *************************/
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
	float3 NormalWS : NORMAL;
	float DepthVS : DEPTH_VIEW_SPACE;
	float2 TexCoord : TEXCOORD0;
	float3 TangentWS : TANGENT;
	float3 BinormalWS : BINORMAL;
};

/*******************  Resources  *************************/
cbuffer CBufferPerFrame : register (b0) {
	float4x4 ViewProj;
	float4x4 View;
	float4x4 World;
	float DisplacementScale;
}

SamplerState TexSampler : register (s0);

Texture2D DisplacementMap : register (t0);

/*******************  Shader  *************************/
[domain("tri")]
DS_OUTPUT main(const HS_CONSTANT_OUTPUT IN, const float3 uvw : SV_DomainLocation, const OutputPatch <HS_OUTPUT, NUM_PATCH_POINTS> patch) {
	DS_OUTPUT OUT = (DS_OUTPUT)0;

	// Compute tex coordinate
	OUT.TexCoord = uvw.x * patch[0].TexCoord + uvw.y * patch[1].TexCoord + uvw.z * patch[2].TexCoord;

	// Compute world normal
	const float3 normalOS = normalize(uvw.x * patch[0].NormalOS + uvw.y * patch[1].NormalOS + uvw.z * patch[2].NormalOS);
	OUT.NormalWS = normalize(mul(float4(normalOS, 0), World).xyz);

	// Compute SV_Position by displacing object position in y coordinate
	float4 posWS = mul(uvw.x * patch[0].PosOS + uvw.y * patch[1].PosOS + uvw.z * patch[2].PosOS, World);
	posWS += float4(OUT.NormalWS * (2 * DisplacementMap.SampleLevel(TexSampler, OUT.TexCoord, 0).x - 1) * DisplacementScale, 0.0f);
	OUT.PosCS = mul(posWS, ViewProj);

	// Compute world tangent and binormal
	OUT.TangentWS = normalize(uvw.x * patch[0].TangentOS + uvw.y * patch[1].TangentOS + uvw.z * patch[2].TangentOS);
	OUT.TangentWS = normalize(mul(float4(OUT.TangentWS, 0), World).xyz);
	OUT.BinormalWS = normalize(cross(OUT.NormalWS, OUT.TangentWS));
	OUT.DepthVS = mul(posWS, View).z;

	return OUT;
}