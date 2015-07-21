#define NUM_PATCH_POINTS 3

struct HullShaderConstantOutput {
	float EdgeFactors[3] : SV_TessFactor;
	float InsideFactors : SV_InsideTessFactor;
};

struct Input {
	float4 PosOS : POSITION;
	float3 NormalOS : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 TangentOS : TANGENT;
};

struct Output {
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
Output main(const HullShaderConstantOutput IN, const float3 uvw : SV_DomainLocation, const OutputPatch <Input, NUM_PATCH_POINTS> patch) {
	Output output = (Output)0;

	output.TexCoord = uvw.x * patch[0].TexCoord + uvw.y * patch[1].TexCoord + uvw.z * patch[2].TexCoord;

	const float3 normalOS = normalize(uvw.x * patch[0].NormalOS + uvw.y * patch[1].NormalOS + uvw.z * patch[2].NormalOS);
	output.NormalVS = normalize(mul(float4(normalOS, 0.0f), WorldView).xyz);

	// Compute SV_Position by displacing object position in y coordinate
	float4 posVS = mul(uvw.x * patch[0].PosOS + uvw.y * patch[1].PosOS + uvw.z * patch[2].PosOS, WorldView);
	// Choose the mipmap level based on distance to the eye; specifically, choose the next miplevel every MipInterval units, and clamp the miplevel in [0,6].
	const float MipInterval = 20.0f;
	const float mipLevel = clamp((length(posVS) - MipInterval) / MipInterval, 0.0f, 6.0f);
	const float displacement = DisplacementMap.SampleLevel(TexSampler, output.TexCoord, mipLevel).x * DisplacementScale;
	// Offset vertex along normal
	posVS += float4(output.NormalVS * displacement, 0.0f);
	output.PosCS = mul(posVS, Proj);

	// Compute world tangent and binormal
	output.TangentVS = normalize(uvw.x * patch[0].TangentOS + uvw.y * patch[1].TangentOS + uvw.z * patch[2].TangentOS);
	output.TangentVS = normalize(mul(float4(output.TangentVS, 0.0f), WorldView).xyz);
	output.BinormalVS = normalize(cross(output.NormalVS, output.TangentVS));

	return output;
}