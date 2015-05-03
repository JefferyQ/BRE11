/*******************  Data  *************************/
#define NUM_PATCH_POINTS 3

struct HS_CONSTANT_OUTPUT {
	float EdgeFactors[3] : SV_TessFactor;
	float InsideFactors : SV_InsideTessFactor;
};

struct HS_OUTPUT {
	float4 PositionL : POSITION;
	float3 NormalL : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 TangentL : TANGENT;
};

struct DS_OUTPUT {
	float4 PositionH : SV_Position;
	float3 PositionW : POSITION;
	float3 NormalW : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 TangentW : TANGENT;
	float3 BinormalW : BINORMAL;
};

/*******************  Resources  *************************/
cbuffer CBufferPerFrame : register (b0) {
	float4x4 ViewProjection;
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
	const float3 normalL = normalize(uvw.x * patch[0].NormalL + uvw.y * patch[1].NormalL + uvw.z * patch[2].NormalL);
	OUT.NormalW = normalize(mul(float4(normalL, 0), World).xyz);

	// Compute SV_Position by displacing local position in y coordinate
	float4 positionW = mul(uvw.x * patch[0].PositionL + uvw.y * patch[1].PositionL + uvw.z * patch[2].PositionL, World);
	positionW += float4(OUT.NormalW * (2 * DisplacementMap.SampleLevel(TexSampler, OUT.TexCoord, 0).x - 1) * DisplacementScale, 0.0f);
	OUT.PositionH = mul(positionW, ViewProjection);

	// Compute world tangent and binormal
	OUT.TangentW = normalize(uvw.x * patch[0].TangentL + uvw.y * patch[1].TangentL + uvw.z * patch[2].TangentL);
	OUT.TangentW = normalize(mul(float4(OUT.TangentW, 0), World).xyz);
	OUT.BinormalW = normalize(cross(OUT.NormalW, OUT.TangentW));

	OUT.PositionW = positionW.xyz;

	return OUT;
}