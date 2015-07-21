#define NUM_PATCH_POINTS 3

struct Input {
	float4 PosOS : POSITION;
	float3 NormalOS : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 TangentOS : TANGENT;
};

struct HullShaderConstantOutput {
	float EdgeFactors[3] : SV_TessFactor;
	float InsideFactors : SV_InsideTessFactor;
};

struct Output {
	float4 PosOS : POSITION;
	float3 NormalOS : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 TangentOS : TANGENT;
};

cbuffer CBufferImmutable : register (b0) {
	float TessellationFactor;
}

HullShaderConstantOutput constant_hull_shader(const InputPatch<Input, NUM_PATCH_POINTS> patch, const uint patchID : SV_PrimitiveID) {
	HullShaderConstantOutput output = (HullShaderConstantOutput)0;
	output.EdgeFactors[0] = TessellationFactor;
	output.EdgeFactors[1] = TessellationFactor;
	output.EdgeFactors[2] = TessellationFactor;
	output.InsideFactors = TessellationFactor;
	return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(NUM_PATCH_POINTS)]
[patchconstantfunc("constant_hull_shader")]
Output main(const InputPatch <Input, NUM_PATCH_POINTS> patch, const uint controlPointID : SV_OutputControlPointID, const uint patchId : SV_PrimitiveID) {
	Output output = (Output)0;
	output.PosOS = patch[controlPointID].PosOS;
	output.NormalOS = patch[controlPointID].NormalOS;
	output.TexCoord = patch[controlPointID].TexCoord;
	output.TangentOS = patch[controlPointID].TangentOS;
	return output;
}