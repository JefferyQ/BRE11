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
	// 3 edge factors + 1 inside factor
	float4 TessellationFactors;
}

HullShaderConstantOutput constant_hull_shader(const InputPatch<Input, NUM_PATCH_POINTS> patch, const uint patchID : SV_PrimitiveID) {
	HullShaderConstantOutput output = (HullShaderConstantOutput)0;
	output.EdgeFactors[0] = TessellationFactors.x;
	output.EdgeFactors[1] = TessellationFactors.y;
	output.EdgeFactors[2] = TessellationFactors.z;
	output.InsideFactors = TessellationFactors.w;
	return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(NUM_PATCH_POINTS)]
[patchconstantfunc("constant_hull_shader")]
Output main(const InputPatch <Input, NUM_PATCH_POINTS> patch, const uint controlPointID : SV_OutputControlPointID) {
	Output output = (Output)0;
	output.PosOS = patch[controlPointID].PosOS;
	output.NormalOS = patch[controlPointID].NormalOS;
	output.TexCoord = patch[controlPointID].TexCoord;
	output.TangentOS = patch[controlPointID].TangentOS;
	return output;
}